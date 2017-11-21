/**
 * \file main_test_fastkernel_double.cpp
 * \brief This is the main file for the test fastkernel_double
 *
 * \date авг. 22, 2015
 * \author alexander
 *
 * \ingroup perf-tests
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/utils/preciseTimer.h"
#include "core/patterndetection/chessBoardCornerDetector.h"
#include "core/buffers/kernels/fastkernel/fastKernel.h"
#include "core/buffers/kernels/sobel.h"
#include "core/buffers/kernels/fastkernel/vectorTraits.h"
#include "core/buffers/kernels/arithmetic.h"
#include "core/buffers/kernels/copyKernel.h"

#include "core/buffers/convolver/convolver.h"

using corecvs::SobelVerticalKernel;
using corecvs::ScalarAlgebraDouble;

using namespace corecvs;

const static unsigned POLUTING_INPUTS = 10;
const static unsigned LIMIT = 50;

const static int  TEST_H_SIZE = 3000;
const static int  TEST_W_SIZE = 4000;

/*const static int  TEST_H_SIZE = 8;
const static int  TEST_W_SIZE = 8;*/


template<typename Type = uint16_t>
class VisiterSemiRandom
{
public:
    void operator() (int y, int x, Type &element)
    {
        element = Type(((unsigned)(y * 54536351 + x * 8769843)));

      //element = Type(1.0); if (y == 3 && x == 3) { element = Type(0.0); }
    }
};

/**
 *  Cool new style (FastKernels) Vertical Sobel filter
 **/
template <typename Algebra>
class VerticalEdgeKernel
{
public:
    static const int inputNumber = 1;
    static const int outputNumber = 1;

    inline static int getCenterX(){ return 1; }
    inline static int getCenterY(){ return 1; }
    inline static int getSizeX()  { return 3; }
    inline static int getSizeY()  { return 3; }

    typedef typename Algebra::InputType       Type;
    typedef typename Algebra::InputSignedType SignedType;

    template<typename OtherAlgebra>
    VerticalEdgeKernel(const VerticalEdgeKernel<OtherAlgebra> & /*other*/) {}

    VerticalEdgeKernel() {}

    inline void process(Algebra &algebra) const
    {
        Type a00 = algebra.getInput(0, 0);
        Type a10 = algebra.getInput(1, 0);
        Type a20 = algebra.getInput(2, 0);
        Type a02 = algebra.getInput(0, 2);
        Type a12 = algebra.getInput(1, 2);
        Type a22 = algebra.getInput(2, 2);

        SignedType negative = SignedType(a00 + a10 + a20);
        SignedType positive = SignedType(a02 + a12 + a22);

        Type result = Type(positive - negative);
        algebra.putOutput(0, 0, result);

        /*Type a00 = algebra.getInput(0,0);
        algebra.putOutput(0,0, a00);*/
    }
};



TEST(FastKernelDouble, testDoubleConvolve)  // TODO: it fails on the line 244!
{
    DpImage* input[POLUTING_INPUTS];

    VisiterSemiRandom<DpImage::InternalElementType> vis;
    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        input[i] = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
        input[i]->touchOperationElementwize(vis);
    }

    DpImage *outputScalar[POLUTING_INPUTS];
    DpImage *outputSimple[POLUTING_INPUTS];
    DpImage *outputVector[POLUTING_INPUTS];
    DpImage *outputKernalized[POLUTING_INPUTS];

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        outputSimple    [i] = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
        outputScalar    [i] = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
        outputVector    [i] = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
        outputKernalized[i] = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
    }

    SYNC_PRINT(("Profiling Vertical edge detection:\n"));
    SYNC_PRINT(("   We will %d times pump [%dx%d] buffers with double elements (%" PRISIZE_T " bytes):\n", LIMIT, TEST_H_SIZE, TEST_W_SIZE, sizeof(double)));
    uint64_t bytesr = LIMIT * TEST_H_SIZE * TEST_W_SIZE * sizeof(double);
    uint64_t bytesw = bytesr;
    SYNC_PRINT(("   Totally read  - %" PRIu64 " bytes = %.2f Mb \n", bytesr, bytesr / 1000000.0));
    SYNC_PRINT(("   Totally write - %" PRIu64 " bytes = %.2f Mb \n", bytesw, bytesw / 1000000.0));

    PreciseTimer start;
    /* Run Simple */
    /* Preparation */
    double duk[] = { -1.0, 0.0, 1.0, -1.0, 0.0, 1.0, -1.0, 0.0, 1.0 };
    DpKernel K(3, 3, duk);

    SYNC_PRINT(("Profiling     Simple Approach:"));
    start = PreciseTimer::currentTime();
    for (unsigned i = 0; i < LIMIT; i++)
    {
        /*delete_safe(outputSimple[i % POLUTING_INPUTS]);
        outputSimple[i % POLUTING_INPUTS] = input[i % POLUTING_INPUTS]->doConvolve<DpImage>(&K);*/

        input[i % POLUTING_INPUTS]->doConvolve<DpImage>(outputSimple[i % POLUTING_INPUTS], &K);
    }
    uint64_t delaySimple = start.usecsToNow();
    SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delaySimple, delaySimple / 1000, delaySimple / LIMIT));

    /* Run kernalized */

#if defined (WITH_SSE)
    SYNC_PRINT(("Profiling Kernalized Approach:"));
    start = PreciseTimer::currentTime();

    ConvolveKernel<DummyAlgebra> convolver(&K, 1, 1);
    BufferProcessor<DpImage, DpImage, ConvolveKernel, VectorAlgebraDouble> procScalar;
    for (unsigned i = 0; i < LIMIT; i++)
    {
        procScalar.process(&input[i % POLUTING_INPUTS], &outputKernalized[i % POLUTING_INPUTS], convolver);
    }
    uint64_t delayKernalized = start.usecsToNow();
    SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delayKernalized, delayKernalized / 1000, delayKernalized / LIMIT));
#endif

    /* Run scalar */
    VerticalEdgeKernel<DummyAlgebra> kernel;

    SYNC_PRINT(("Profiling     Scalar Approach:"));
    start = PreciseTimer::currentTime();
    BufferProcessor<DpImage, DpImage, VerticalEdgeKernel, ScalarAlgebraDouble> procKernalized;
    for (unsigned i = 0; i < LIMIT; i++)
    {
        DpImage *cinput [1] = { input       [i % POLUTING_INPUTS] };
        DpImage *coutput[1] = { outputScalar[i % POLUTING_INPUTS] };

        procKernalized.process(cinput, coutput, kernel);
    }
    uint64_t delayScalar = start.usecsToNow();
    SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delayScalar, delayScalar / 1000, delayScalar / LIMIT));


    /* Run vector */
#if defined (WITH_SSE)
    CopyKernel<DummyAlgebra> kernel1;
    SYNC_PRINT(("Profiling     Vector Approach:"));
    start = PreciseTimer::currentTime();
    BufferProcessor<DpImage, DpImage, CopyKernel, VectorAlgebraDouble> procVector;
    for (unsigned i = 0; i < LIMIT; i++) {
        DpImage *cinput [1] = { input       [i % POLUTING_INPUTS] };
        DpImage *coutput[1] = { outputVector[i % POLUTING_INPUTS] };

        procVector.process(cinput, coutput, kernel1);
    }
    uint64_t delayVector = start.usecsToNow();
    SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delayVector, delayVector / 1000, delayVector / LIMIT));
#endif

    /* Report bandwidth estimation*/

#if defined (WITH_SSE)
    SYNC_PRINT(("Kernalized Bandwidth used is %.2lf bytes/s = %.2lf Mb/s = %.2lf Gb/s \n",
        (double)(bytesr + bytesw) / delayKernalized * 1000000.0,
        (double)(bytesr + bytesw) / delayKernalized,
        (double)(bytesr + bytesw) / delayKernalized / 1000.0
        ));

    SYNC_PRINT(("Scalar     Bandwidth used is %.2lf bytes/s = %.2lf Mb/s = %.2lf Gb/s \n",
        (double)(bytesr + bytesw) / delayScalar * 1000000.0,
        (double)(bytesr + bytesw) / delayScalar,
        (double)(bytesr + bytesw) / delayScalar / 1000.0
        ));

    SYNC_PRINT(("Vector     Bandwidth used is %.2lf bytes/s = %.2lf Mb/s = %.2lf Gb/s \n",
        (double)(bytesr + bytesw) / delayVector * 1000000.0,
        (double)(bytesr + bytesw) / delayVector,
        (double)(bytesr + bytesw) / delayVector / 1000.0
        ));
#endif

    /* Print results*/
    if (TEST_H_SIZE < 15 && TEST_W_SIZE < 15)
    {
        cout << *input[0] << endl;

        cout << "Simple result:" << endl;
        cout << *outputSimple[0] << endl;
        cout << "Scalar result:" << endl;
        cout << *outputScalar[0] << endl;
#if defined (WITH_SSE)
        cout << "Vector result:" << endl;
        cout << *outputVector[0] << endl;
#endif
    }


    /* Check equality */
#if defined (WITH_SSE) || defined (WITH_AVX)
    for (int i = 0; i < TEST_H_SIZE; i++)
    {
        for (int j = 0; j < TEST_W_SIZE; j++)
        {
            // the results must be different as different kernels were applied!
            //CORE_ASSERT_DOUBLE_EQUAL_EP(outputScalar[0]->element(i, j), outputVector[0]->element(i, j), 1e-5,
            //    ("Scalar and Vector computation results are not equal at (%d, %d) (%lf vs %lf).\n",
            //    i, j, outputScalar[0]->element(i, j), outputVector[0]->element(i, j)));
        }
    }

    for (int i = 0; i < TEST_H_SIZE; i++)
    {
        for (int j = 0; j < TEST_W_SIZE; j++)
        {
            CORE_ASSERT_DOUBLE_EQUAL_EP(outputScalar[0]->element(i, j), outputKernalized[0]->element(i, j), 1e-5,
                ("Scalar and Vector computation results are not equal at (%d, %d) (%lf vs %lf).\n",
                i, j, outputScalar[0]->element(i, j), outputKernalized[0]->element(i, j)));
        }
    }
#endif

    for (int i = 1; i < TEST_H_SIZE - 1; i++)
    {
        for (int j = 1; j < TEST_W_SIZE - 1; j++)
        {
            CORE_ASSERT_DOUBLE_EQUAL_EP(outputScalar[0]->element(i, j), outputSimple[0]->element(i, j), 1e-5,
                ("Scalar and Simple computation results are not equal at (%d, %d) (%lf vs %lf).\n",
                i, j, outputScalar[0]->element(i, j), outputSimple[0]->element(i, j)));
        }
    }


    /* Cleanup */
    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
        delete_safe(input[i]);

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        delete_safe(outputScalar[i]);
        delete_safe(outputVector[i]);
        delete_safe(outputKernalized[i]);
        delete_safe(outputSimple[i]);
    }
}


TEST(FastKernelDouble, testLargeKernel)
{
    DpImage * input[POLUTING_INPUTS];
    PreciseTimer start;

    VisiterSemiRandom<DpImage::InternalElementType> vis;
    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        input[i] = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
        input[i]->touchOperationElementwize(vis);
    }

    DpImage *outputKernalized[POLUTING_INPUTS];

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        outputKernalized[i] = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
    }

    for (int size = 3; size < 20; size += 4)
    {
        DpImage *kernel = new DpImage(size, size);
        kernel->touchOperationElementwize(vis);

#if defined (WITH_SSE)
        SYNC_PRINT(("Profiling Kernalized Approach [%dx%d]:", kernel->w, kernel->h));
        start = PreciseTimer::currentTime();

        ConvolveKernel<DummyAlgebra> convolver(kernel, kernel->h / 2, kernel->w / 2);
        BufferProcessor<DpImage, DpImage, ConvolveKernel, VectorAlgebraDouble> procScalar;
        for (unsigned i = 0; i < LIMIT; i++) {
            procScalar.process(&input[i % POLUTING_INPUTS], &outputKernalized[i % POLUTING_INPUTS], convolver);
        }
        uint64_t delayKernalized = start.usecsToNow();
        SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delayKernalized, delayKernalized / 1000, delayKernalized / LIMIT));
#endif

        delete_safe(kernel);
    }

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        delete_safe(input[i]);
    }

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        delete_safe(outputKernalized[i]);
    }
}

struct TestDescr {
    Convolver::ConvolverImplementation imp;
    int runs;
    int ksize;
    const char *name;

    bool check;
    DpImage *result;
    FpImage *result1;
    uint64_t delay;
};

template<typename Type = uint16_t>
class VisiterSemiRandom1
{
public:
    void operator() (int y, int x, Type &element)
    {
        element = Type(((unsigned)(y * 0.54536351 + x * 0.8769843)));

      //element = Type(1.0); if (y == 3 && x == 3) { element = Type(0.0); }
    }
};


TEST(FastKernelDouble, testConvolver)
{
    DpImage * input[POLUTING_INPUTS];
    PreciseTimer start;

    SYNC_PRINT(("We will profile buffers [%dx%d]. We will have %d polluting inputs\n\n", TEST_H_SIZE, TEST_W_SIZE, POLUTING_INPUTS));

    VisiterSemiRandom1<DpImage::InternalElementType> vis;
    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        input[i] = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
        input[i]->touchOperationElementwize(vis);
    }

    DpImage *output[POLUTING_INPUTS];

    /*ok algos to test*/

    const int kernelSize = 11;

    TestDescr tests[] = {
          { Convolver::ALGORITHM_SSE_DMITRY                 , 150, 5, "Dmitry"  , true, NULL, NULL, 0 }

        , { Convolver::ALGORITHM_NAIVE                      ,  20, 5, "Naive", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_1               , 150, 5, "unroll 1", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_2               , 150, 5, "unroll 2", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_3               , 150, 5, "unroll 3", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_4               , 150, 5, "unroll 4", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_5               , 150, 5, "unroll 5", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_6               , 150, 5, "unroll 6", false, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_7               , 150, 5, "unroll 7", false, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_8               , 150, 5, "unroll 8", false, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_9               , 150, 5, "unroll 9", false, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_10              , 150, 5, "unroll10", false, NULL, NULL, 0 }
        
        , { Convolver::ALGORITHM_SSE_UNROLL_12              , 150, 5, "unroll12", false, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_16              , 150, 5, "unroll16", false, NULL, NULL, 0}
        
        , { Convolver::ALGORITHM_SSE_UNROLL_20              , 150, 5, "unroll20", false, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_40              , 150, 5, "unroll40", false, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_UNROLL_100             , 150, 5, "unroll100", false, NULL, NULL, 0 }

        , { Convolver::ALGORITHM_SSE_FASTKERNEL             ,  60, 5, "Fastkernel", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_FASTKERNEL_EXP         ,  60, 5, "FastkernelE", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_FASTKERNEL_EXP5        ,  60, 5, "FastkernelE5", true, NULL, NULL, 0 }

        , { Convolver::ALGORITHM_SSE_WRAPPERS               ,  60, 5, "Wrappers", true, NULL, 0 }

        , { Convolver::ALGORITHM_SSE_WRAPPERS_UNROLL_1      , 150, 5, "Wrappers u1", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_WRAPPERS_UNROLL_5      , 150, 5, "Wrappers u5", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_WRAPPERS_UNROLL_10     , 150, 5, "Wrappers u10", false, NULL, NULL, 0 }

        , { Convolver::ALGORITHM_SSE_WRAPPERS_EX_UNROLL_1   , 150, 5, "Wrap Ex u1", true, NULL, NULL, 0 }
        , { Convolver::ALGORITHM_SSE_WRAPPERS_EX_UNROLL_2   , 150, 5, "Wrap Ex u2", true, NULL, NULL, 0 }
    };



    /* Results are stored to compare */
    for (size_t i = 0; i < CORE_COUNT_OF(tests); i++) {
     //   tests[i].result = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
        tests[i].runs *= 2;
    }

    /* Cache polluting outputs */
    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        output[i] = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
    }

    /* Main cycle */
    for (size_t t = 0; t < CORE_COUNT_OF(tests); t++)
    {
        TestDescr &test = tests[t];
        DpKernel *kernel = new DpKernel(kernelSize, kernelSize);

        /* flops */
        double flop   = 2.0 * (double)TEST_H_SIZE * TEST_W_SIZE * kernelSize * kernelSize;
        double gflops = flop / 1000000.0 / 1000.0;

        kernel->touchOperationElementwize(vis);
        for (unsigned i = 0; i < POLUTING_INPUTS; i++)
        {
            output[i]->fillWith(0.0);
        }

        SYNC_PRINT(("Profiling %15s Approach [%dx%d] (%3d runs)", test.name, kernel->w, kernel->h, test.runs));
        start = PreciseTimer::currentTime();

        for (int j = 0; j < test.runs; j++) {
            Convolver::convolve(*input[j % POLUTING_INPUTS], *kernel, *output[j % POLUTING_INPUTS], test.imp);
        }

        uint64_t delay = start.usecsToNow();

        uint64_t odelay = delay / test.runs;
        test.delay = odelay;
        uint64_t sodelay = tests[0].delay;

        double runss = 1000000.0 / ((double)delay / test.runs);
        double gflopss = runss * gflops;

        test.result = new DpImage(output[0]);

        SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us %3.2lf%% | % 7.3lf Gflops/s |\n",
                    delay, delay / 1000, delay / test.runs, odelay * 100.0 / sodelay, gflopss));

        delete_safe(kernel);
    }

    /*Check the results */
    SYNC_PRINT(("Checking equality... \n"));

    int stepoff = 20;
    for (size_t t = 1; t < CORE_COUNT_OF(tests); t++)
    {
        DpImage *b1 = tests[0].result;
        DpImage *b2 = tests[t].result;

        if (!tests[t].check) {
            continue;
        }

        SYNC_PRINT(("-> <%s> and <%s> \n", tests[0].name, tests[t].name));

        for (int i = stepoff; i < TEST_H_SIZE - stepoff; i++)
        {
            for (int j = stepoff; j < TEST_W_SIZE - stepoff; j++)
            {
                CORE_ASSERT_DOUBLE_EQUAL_EP(b1->element(i, j), b2->element(i, j), 1e-2,
                    ("Computation results <%s> and <%s> are not equal at (%d, %d) (%lf vs %lf).\n",
                     tests[0].name, tests[t].name,
                    i, j, b1->element(i, j), b2->element(i, j)));
            }
        }
    }
    SYNC_PRINT(("done\n"));



    /*Cleanup*/
    for (size_t i = 0; i < CORE_COUNT_OF(tests); i++) {
        delete_safe(tests[i].result);
    }

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        delete_safe(input[i]);
    }

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        delete_safe(output[i]);
    }
}



TEST(FastKernelFloat, testConvolver)
{
    FpImage * input[POLUTING_INPUTS];
    PreciseTimer start;

    SYNC_PRINT(("We will profile buffers [%dx%d]. We will have %d polluting inputs\n\n", TEST_H_SIZE, TEST_W_SIZE, POLUTING_INPUTS));

    VisiterSemiRandom1<FpImage::InternalElementType> vis;
    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        input[i] = new FpImage(TEST_H_SIZE, TEST_W_SIZE);
        input[i]->touchOperationElementwize(vis);
    }

    FpImage *output[POLUTING_INPUTS];

    /*ok algos to test*/

    const int kernelSize = 11;

    TestDescr tests[] = {
 //       {Convolver::ALGORITHM_SSE_DMITRY     , 150, 5, "Dmitry"  , true, NULL, 0},

        {Convolver::ALGORITHM_NAIVE          ,  20, 5, "Naive"   , true, NULL, NULL, 0},

        {Convolver::ALGORITHM_SSE_UNROLL_1   , 150, 5, "unroll 1", true, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_2   , 150, 5, "unroll 2", true, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_3   , 150, 5, "unroll 3", true, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_4   , 150, 5, "unroll 4", true, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_5   , 150, 5, "unroll 5", true, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_6   , 150, 5, "unroll 6", false, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_7   , 150, 5, "unroll 7", false, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_8   , 150, 5, "unroll 8", false, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_9   , 150, 5, "unroll 9", false, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_10  , 150, 5, "unroll10", false, NULL, NULL, 0},

        {Convolver::ALGORITHM_SSE_UNROLL_12  , 150, 5, "unroll12", false, NULL, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_16  , 150, 5, "unroll16", false, NULL, NULL, 0},


/*        {Convolver::ALGORITHM_SSE_UNROLL_20   , 150, 5, "unroll20", false, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_40   , 150, 5, "unroll40", false, NULL, 0},
        {Convolver::ALGORITHM_SSE_UNROLL_100  , 150, 5, "unroll100", false, NULL, 0},

        {Convolver::ALGORITHM_SSE_FASTKERNEL     ,  60, 5, "Fastkernel", true, NULL, 0},
        {Convolver::ALGORITHM_SSE_FASTKERNEL_EXP ,  60, 5, "FastkernelE", true, NULL, 0},*/
        /*{Convolver::ALGORITHM_SSE_FASTKERNEL_EXP5,  60, 5, "FastkernelE5", true, NULL, 0},*/

        /*{Convolver::ALGORITHM_SSE_WRAPPERS   ,  60, 5, "Wrappers", true, NULL, 0},*/

/*        {Convolver::ALGORITHM_SSE_WRAPPERS_UNROLL_1   , 150, 5, "Wrappers u1", true, NULL, 0},
        {Convolver::ALGORITHM_SSE_WRAPPERS_UNROLL_5   , 150, 5, "Wrappers u5", true, NULL, 0},
        {Convolver::ALGORITHM_SSE_WRAPPERS_UNROLL_10  , 150, 5, "Wrappers u10", false, NULL, 0},*/

/*        {Convolver::ALGORITHM_SSE_WRAPPERS_EX_UNROLL_1, 150, 5, "Wrap Ex u1", true, NULL, 0},
        {Convolver::ALGORITHM_SSE_WRAPPERS_EX_UNROLL_2, 150, 5, "Wrap Ex u2", true, NULL, 0},*/


    };



    /* Results are stored to compare */
    for (size_t i = 0; i < CORE_COUNT_OF(tests); i++) {
     //   tests[i].result = new DpImage(TEST_H_SIZE, TEST_W_SIZE);
        tests[i].runs *= 2;
    }

    /* Cache polluting outputs */
    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        output[i] = new FpImage(TEST_H_SIZE, TEST_W_SIZE);
    }

    /* Main cycle */
    for (size_t t = 0; t < CORE_COUNT_OF(tests); t++)
    {
        TestDescr &test = tests[t];
        FpKernel *kernel = new FpKernel(kernelSize, kernelSize);

        /* flops */
        double flop   = 2.0 * (double)TEST_H_SIZE * TEST_W_SIZE * kernelSize * kernelSize;
        double gflops = flop / 1000000.0 / 1000.0;

        kernel->touchOperationElementwize(vis);
        for (unsigned i = 0; i < POLUTING_INPUTS; i++)
        {
            output[i]->fillWith(0.0);
        }

        SYNC_PRINT(("Profiling %15s Approach [%dx%d] (%3d runs)", test.name, kernel->w, kernel->h, test.runs));
        start = PreciseTimer::currentTime();

        for (int j = 0; j < test.runs; j++) {
            Convolver::convolve(*input[j % POLUTING_INPUTS], *kernel, *output[j % POLUTING_INPUTS], test.imp);
        }

        uint64_t delay = start.usecsToNow();

        uint64_t odelay = delay / test.runs;
        test.delay = odelay;
        uint64_t sodelay = tests[0].delay;

        double runss = 1000000.0 / ((double)delay / test.runs);
        double gflopss = runss * gflops;

        test.result1 = new FpImage(output[0]);

        SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us %3.2lf%% | % 7.3lf Gflops/s |\n",
                    delay, delay / 1000, delay / test.runs, odelay * 100.0 / sodelay, gflopss));

        delete_safe(kernel);
    }

    /*Check the results */
    SYNC_PRINT(("Checking equality... \n"));

    int stepoff = 40;
    for (size_t t = 1; t < CORE_COUNT_OF(tests); t++)
    {
        FpImage *b1 = tests[0].result1;
        FpImage *b2 = tests[t].result1;

        if (!tests[t].check) {
            continue;
        }

        SYNC_PRINT(("-> <%s> and <%s> \n", tests[0].name, tests[t].name));

        for (int i = stepoff; i < TEST_H_SIZE - stepoff; i++)
        {
            for (int j = stepoff; j < TEST_W_SIZE - stepoff; j++)
            {
                CORE_ASSERT_DOUBLE_EQUAL_EP(b1->element(i, j), b2->element(i, j), 1e-2,
                    ("Computation results <%s> and <%s> are not equal at (%d, %d) (%lf vs %lf).\n",
                     tests[0].name, tests[t].name,
                    i, j, b1->element(i, j), b2->element(i, j)));
            }
        }
    }
    SYNC_PRINT(("done\n"));



    /*Cleanup*/
    for (size_t i = 0; i < CORE_COUNT_OF(tests); i++) {
        delete_safe(tests[i].result);
    }

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        delete_safe(input[i]);
    }

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        delete_safe(output[i]);
    }

}
