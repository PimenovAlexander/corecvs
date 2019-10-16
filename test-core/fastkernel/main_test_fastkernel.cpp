/**
 * \file main_test_fastkernel.cpp
 * \brief This is the main file for the test fastkernel
 *
 * \date Sep 26, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#include <iostream>
#include <stdint.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/kernels/fastkernel/scalarAlgebra.h"
#include "core/buffers/kernels/fastkernel/vectorAlgebra.h"
#include "core/buffers/kernels/fastkernel/fastKernel.h"
#include "core/math/vector/vector3d.h"
#include "core/buffers/kernels/gaussian.h"
#include "core/buffers/kernels/copyKernel.h"
#include "core/buffers/kernels/sobel.h"
#include "core/utils/preciseTimer.h"
#include "core/buffers/kernels/arithmetic.h"
#include "core/buffers/kernels/threshold.h"
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/integralBuffer.h"
#include "core/buffers/morphological/morphological.h"
#include "core/buffers/kernels/fastkernel/vectorTraits.h"
#include "../../core/buffers/kernels/logicKernels.h"
#include "../../core/buffers/rgb24/abstractPainter.h"

using namespace corecvs;

/**
 *  Have a look at this powerful jedi technique
 **/
template<typename Type = uint16_t>
class Visiter50000
{
public:
    void operator() (int /* y */, int x , Type &element) {
        element = Type((x % 5) ? 0 : 5);
    };
};

const static int TEST_H_SMALL_SIZE = 5;
const static int TEST_W_SMALL_SIZE = 20;

/**
 *  Have a look at this powerful jedi technique
 **/
template<typename Type = uint16_t>
class VisiterSemiRandom
{
public:
    void operator() (int y , int x , Type &element) {
        element = Type(((unsigned)(y * 54536351 + x * 8769843 + 5)) % (G12Buffer::BUFFER_MAX_VALUE + 1));
    }
};

TEST(FastKernel, _testScalar)
{
    printf("Testing the fast kernel infrastructure with integer scalar\n");
    G12Buffer *input = new G12Buffer(TEST_H_SMALL_SIZE,TEST_W_SMALL_SIZE);
    G12Buffer *output = new G12Buffer(TEST_H_SMALL_SIZE,TEST_W_SMALL_SIZE);
    //Visiter50000<> vis;
    VisiterSemiRandom<> vis;
    input->touchOperationElementwize(vis);
    printf("Input:\n");
    input->print();

    G12Buffer *in[2] = {input, input};
    BufferProcessor<G12Buffer, G12Buffer, EdgeMagnitude, G12BufferAlgebraScalar> proc;
    proc.process(in, &output);
    printf("Scalar:\n");
    output->print();

    delete input;
    delete output;
}

template<int inputNumber, int outputNumber>
class ScalarAlgebraVector33
{
public:
    typedef TraitGeneric<Vector3dd> TraitVector3dd;
    typedef ScalarAlgebraMulti<TraitVector3dd, TraitVector3dd, inputNumber, outputNumber> Type;
};

TEST(FastKernel, _testVector3dd)
{
    printf("Testing the fast kernel infrastructure with Vector3dd\n");
    typedef AbstractBuffer<Vector3dd> BufferType;
    BufferType *input  = new BufferType(TEST_H_SMALL_SIZE,TEST_W_SMALL_SIZE);
    BufferType *output = new BufferType(TEST_H_SMALL_SIZE,TEST_W_SMALL_SIZE);

    Visiter50000<Vector3dd> vis;
    input->touchOperationElementwize(vis);
    cout << *input;

    BufferProcessor<BufferType, BufferType, SobelHorizontalKernel, ScalarAlgebraVector33> proc;
    proc.process(&input, &output);
    cout << *output;
    delete input;
    delete output;
}

#ifdef WITH_SSE


TEST(FastKernel, MulAddKernel)
{
    DpImage image(5,5);

    VisiterSemiRandom<double> vis;
    image.touchOperationElementwize(vis);
    cout << "Before:" << endl;
    cout << image;

    MulAddConstKernel<DummyAlgebra> kernel(1000.0, 0.1);
    BufferProcessor<DpImage, DpImage, MulAddConstKernel, ScalarAlgebraDouble> proc;
    DpImage *inout [1] = {&image};
    proc.process(inout, inout, kernel);
    cout << "After2:" << endl;
    cout << image;

}


TEST(FastKernel, _testSSE)
{
    printf("Testing the fast kernel infrastructure with SSE\n");
    G12Buffer *input = new G12Buffer(TEST_H_SMALL_SIZE,TEST_W_SMALL_SIZE);
    G12Buffer *output = new G12Buffer(TEST_H_SMALL_SIZE,TEST_W_SMALL_SIZE);

    VisiterSemiRandom<> vis;
//    Visiter50000<> vis;
    input->touchOperationElementwize(vis);
//    input->print();

    G12Buffer *in[2] = {input, input};
    BufferProcessor<G12Buffer, G12Buffer, EdgeMagnitude, G12BufferAlgebra> proc;
    proc.process(in, &output);
    printf("Vector:\n");
    output->print();

    proc.processSaveAligned(in, &output);
    printf("Aligned Save:\n");
    output->print();

    BufferProcessor<G12Buffer, G12Buffer, EdgeMagnitude, G12BufferAlgebraStreaming> procS;
    printf("Streaming:\n");
    procS.processSaveAligned(in, &output);
    output->print();

    delete input;
    delete output;
}

/*
void _profileManualAddStream (void)
{
     int const TEST_H_SIZE = 600;
     int const TEST_W_SIZE = 1100;
     PreciseTimer start;
     uint64_t delay;

     G12Buffer * input1[POLUTING_INPUTS];
     G12Buffer * input2[POLUTING_INPUTS];

     Visiter50000<> vis;
     for (unsigned i = 0; i < POLUTING_INPUTS; i++)
     {
         input1[i] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
         input1[i]->touchOperationElementwize(vis);
         input2[i] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
         input2[i]->touchOperationElementwize(vis);
     }

     G12Buffer *output = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);

     printf("Profiling Manual Kernel       Sob H    :");
     start = PreciseTimer::currentTime();

     for (unsigned k = 0; k < LIMIT; k++) {
         CG12Buffer *in = input[k % POLUTING_INPUTS];
         for (int i = 0; i < in->h - 1; i++)
         {
             uint16_t *start = &(in->element(i - 1, 0));
             for (int j = 1; j < in->w - 1; j++)
             {
                 uint16_t a = *(start);
                 uint16_t b = *(start + in->stride);
                 uint16_t c = *(start + 2 * in->stride);

                 uint16_t a1 = *(start + 2);
                 uint16_t b1 = *(start + in->stride + 2);
                 uint16_t c1 = *(start + 2 * in->stride + 2);
                 output->element(i,j) = (a + 2 * (b - b1) + c - a1 - c1) >> 3;
                 start++;
             }
         }
     }
     delay = start.usecsToNow();
     printf("%8" PRIu64 "us %8" PRIu64 "ms\n", delay, delay / 1000); fflush(stdout);
}
*/

TEST(FastKernel, testEdgeDetector)
{
    G12Buffer *input = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    if (input == nullptr)
    {
        cout << "Could not open test image" << endl;
        return;
    }
    BufferProcessor<G12Buffer, G12Buffer, EdgeMagnitude, G12BufferAlgebra> processor;
    G12Buffer *edges = new G12Buffer(input->h, input->w);

    processor.process(&input, &edges);
    G12IntegralBuffer *integral = new G12IntegralBuffer(edges);
    G12Buffer *blurred = integral->rectangularBlur<G12Buffer>(1,1);
    G12Buffer *output = blurred->binarize(100);


    BMPLoader().save("edges.bmp", output);

    delete output;
    delete blurred;
    delete integral;
    delete edges;
    delete input;
}

TEST(FastKernel, profileEdgeDetector)  // it could be moved to perf-tests...
{
    const static unsigned int LIMIT = 100;
    G12Buffer *inputs[LIMIT];
    uint64_t delay;

    for (unsigned i = 0; i < LIMIT; i++)
    {
        inputs[i] = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
        if (inputs[i] == nullptr)
        {
            cout << "Could not open test image" << endl;
            return;
        }
    }

    BufferProcessor<G12Buffer, G12Buffer, EdgeMagnitude, G12BufferAlgebra> processor;
    PreciseTimer start = PreciseTimer::currentTime();

    for (unsigned i = 0; i < LIMIT; i++)
    {
        G12Buffer *edges = new G12Buffer(inputs[i]->h, inputs[i]->w, false);
        processor.process(&inputs[i], &edges);
        G12IntegralBuffer *integral = new G12IntegralBuffer(edges);
        G12Buffer *blurred = integral->rectangularBlur<G12Buffer>(5,5);
        G12Buffer *output = blurred->binarize(100);
        delete edges;
        delete integral;
        delete blurred;
        delete inputs[i];
        delete output;
    }
    delay = start.usecsToNow();

    printf("Edge detection %8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us %8" PRIu64 "ms\n",
            delay,
            (uint64_t)fround((double)delay / 1000.0),
            (uint64_t)fround((double)delay / LIMIT),
            (uint64_t)fround((double)delay / 1000.0 / LIMIT));
    fflush(stdout);
}

TEST(FastKernel, testSSEMath)
{
    uint16_t valueT = -(1 < 2);
    uint16_t valueF =   2 < 1;

    printf ("True is %x, False is %d\n", valueT, valueF);

    Int16x8 result = SSEMath::mul<2>(Int16x8(2));
    cout << result << endl;
    cout << G12BufferAlgebra<1,1>::Type::mul<2>(Int16x8(2)) << endl;

    cout << Int16x8(1,2,3,4,5,6,7,8).expand() << endl;

    for (unsigned i = 0; i < 0x5000; i++)
    {
        UInt16x8 input((uint16_t)i);
        //cout << input << endl;
        UInt16x8 output = SSEMath::div<5>(input);

        uint16_t inputS = i;
        uint16_t outputS = GenericMath<uint16_t>::div<5>(inputS);
        CORE_ASSERT_TRUE_P(output[0] == outputS, ("Problem with %d s=%d v=%d", i, outputS, output[0]));
    }
}

TEST(FastKernel, testSSEMul)
{
    uint32_t data1[4] = { 12, 554, 345, 654 };
    uint32_t data2[4] = { 12, 554, 345, 654 };

    Int32x4 in1(data1);
    Int32x4 in2(data2);

    Int32x4 result = in1 * in2;
    cout << "Products are "<< result << endl;
    for (unsigned i = 0; i < CORE_COUNT_OF(data1); i++)
    {
        CORE_ASSERT_TRUE_P((uint)result[i] == data1[i] * data2[i], ("Problem with product"));
    }
}

#endif // WITH_SSE


TEST(FastKernel, testBooleanOperations)
{
    G8Buffer *mask1 = new G8Buffer(100, 100);
    G8Buffer *mask2 = new G8Buffer(100, 100);

    AbstractPainter<G8Buffer> painter1(mask1);
    AbstractPainter<G8Buffer> painter2(mask2);

    painter1.drawCircle(33,33, 50, 255);
    painter2.drawCircle(66,66, 50, 255);

    /* So */
    RGB24Buffer *in1 = new RGB24Buffer(mask1->getSize());
    RGB24Buffer *in2 = new RGB24Buffer(mask2->getSize());
    in1->drawG8Buffer(mask1);
    in2->drawG8Buffer(mask2);
    BMPLoader().save("in1.bmp", in1);
    BMPLoader().save("in2.bmp", in2);

    RGB24Buffer *out = new RGB24Buffer(mask1->getSize());
    G8Buffer *inarr[2] = {mask1, mask2};

    G8Buffer *andmask = new G8Buffer(mask1->getSize());
    const LogicAnd<DummyAlgebra> andkernel = LogicAnd<DummyAlgebra>();
    BufferProcessor<G8Buffer, G8Buffer, LogicAnd, G8BufferAlgebra > processorAnd;
    processorAnd.process(inarr, &andmask, andkernel);
    out->drawG8Buffer(andmask);
    BMPLoader().save("and.bmp", out);

    G8Buffer *ormask = new G8Buffer(mask1->getSize());
    const LogicOr<DummyAlgebra> orkernel = LogicOr<DummyAlgebra>();
    BufferProcessor<G8Buffer, G8Buffer, LogicOr, G8BufferAlgebra > processorOr;
    processorOr.process(inarr, &ormask, orkernel);
    out->drawG8Buffer(ormask);
    BMPLoader().save("or.bmp", out);

    G8Buffer *xormask = new G8Buffer(mask1->getSize());
    const LogicXor<DummyAlgebra> xorkernel = LogicXor<DummyAlgebra>();
    BufferProcessor<G8Buffer, G8Buffer, LogicXor, G8BufferAlgebra > processorXor;
    processorXor.process(inarr, &xormask, xorkernel);
    out->drawG8Buffer(xormask);
    BMPLoader().save("xor.bmp", out);

    G8Buffer *subtmask = new G8Buffer(mask1->getSize());
    const LogicSubt<DummyAlgebra> subtkernel = LogicSubt<DummyAlgebra>();
    BufferProcessor<G8Buffer, G8Buffer, LogicSubt, G8BufferAlgebra > processorSubt;
    processorSubt.process(inarr, &subtmask, subtkernel);
    out->drawG8Buffer(subtmask);
    BMPLoader().save("subt.bmp", out);


    delete out;
    delete in2;
    delete in1;
    delete subtmask;
    delete xormask;
    delete ormask;
    delete andmask;
    delete mask2;
    delete mask1;
}


#if 0
int main (int /*argC*/, char ** /*argV*/)
{
    testSSEMul();
    //testBooleanOperations ();
    return 0;

//int main (int /*argC*/, char ** /*argV*/)
//{
//    testBooleanOperations ();
//    return 0;
//#ifdef WITH_SSE
//	_testFastKernel();
//	_testFastKernelSSE();



//#ifdef PROFILE_ACCESS_ALIGNMENT
//    Int16x8::resetAlignmnetCouters();
//#endif


//#ifdef PROFILE_ACCESS_ALIGNMENT
//	uint64_t total     = Int16x8::alignedUWrites + Int16x8::alignedAWrites + Int16x8::unalignedUWrites + Int16x8::streamedWrites;
//	uint64_t dquwrites = Int16x8::alignedUWrites + Int16x8::unalignedUWrites;
//	printf("Total accesses          : %llu\n", total);
//	printf("Unaligned instructions  : %12llu %7.2lf%%\n", dquwrites, dquwrites * 100.0 / total);
//	printf("   Unaligned accesses   : %12llu %7.2lf%%\n", Int16x8::unalignedUWrites, Int16x8::unalignedUWrites * 100.0 / total);
//	printf("   Aligned   accesses   : %12llu %7.2lf%%\n", Int16x8::alignedUWrites, Int16x8::alignedUWrites * 100.0 / total);
//	printf("Aligned   instructions  : %12llu %7.2lf%%\n", Int16x8::alignedAWrites, Int16x8::alignedAWrites * 100.0 / total);
//	printf("Streamed  instructions  : %12llu %7.2lf%%\n", Int16x8::streamedWrites, Int16x8::streamedWrites * 100.0 / total);

//	Int16x8::resetAlignmnetCouters();
//#endif


//#endif // WITH_SSE
//    cout << "PASSED" << endl;
//    return 0;
//}

#endif
