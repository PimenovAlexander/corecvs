/**
 * \file main_test_fastkernel_profile.cpp
 * \brief This is the main file for the test fastkernel_profile 
 *
 * \date Sep 13, 2012
 * \author apimenov
 *
 * \ingroup perf-tests
 */

#include <iostream>
#include <string.h>
#include <stdint.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "core/buffers/g12Buffer.h"
#include "core/math/sse/sseWrapper.h"
#include "core/utils/preciseTimer.h"

#include "core/buffers/kernels/gaussian.h"
#include "core/buffers/kernels/laplace.h"
#include "core/buffers/kernels/sobel.h"
#include "core/buffers/kernels/copyKernel.h"
#include "core/buffers/kernels/arithmetic.h"
#include "core/buffers/morphological/morphological.h"
#include "core/buffers/kernels/threshold.h"

#include "core/buffers/kernels/fastkernel/baseAlgebra.h"
#include "core/buffers/kernels/fastkernel/fastKernel.h"
#include "core/buffers/kernels/fastkernel/vectorTraits.h"

#ifdef WITH_SSE
using corecvs::Int32x4;
using corecvs::G12BufferAlgebraStreaming;
#endif

using corecvs::G12Buffer;
using corecvs::PreciseTimer;
using corecvs::Blur5Horisontal;
using corecvs::SobelHorizontalKernel;
using corecvs::SumBuffers;
using corecvs::Blur5Vertical;
using corecvs::DummyAlgebra;
using corecvs::BufferProcessor;
using corecvs::ScalarAlgebraUInt16_t;
using corecvs::DilateKernel;
using corecvs::ErodeKernel;
using corecvs::G12BufferAlgebra;
using corecvs::CopyKernel;
using corecvs::EdgeMagnitude;
using corecvs::SobelVerticalKernel;
using corecvs::Gaussian3x3Kernel;
using corecvs::Laplacian3x3Kernel;
using corecvs::SubtractBuffers;
using corecvs::MixBuffers;
using corecvs::DifferenceBuffers;
using corecvs::ThresholdBinariseKernel;
using corecvs::G12BufferAlgebraScalar;

const static unsigned POLUTING_INPUTS = 50;
const static unsigned LIMIT = 300;

const static int  TEST_H_SIZE = 600;
const static int  TEST_W_SIZE = 800;


/**
 *  Have a look at this powerful jedi technique
 **/
template<typename Type = uint16_t>
class VisiterSemiRandom
{
public:
    void operator() (int y , int x , Type &element) {
        element = Type(((unsigned)(y * 54536351 + x * 8769843)) % (G12Buffer::BUFFER_MAX_VALUE + 1));
    }
};

/**
 *  This function profiles the access speed to the memory when working with large blocks
 **/
void _profileAlgnmentAccessSpeed( void )
{

#ifdef WITH_SSE
    std::cout << "This test profile sse access speed depending on the alignment" << std::endl;
    //  800Mb
    //                           '  '
    static const int LIMIT =  400000000;
    PreciseTimer start;
    uint64_t delay;


    uint32_t *data1 = new uint32_t[LIMIT + 100];
    uint32_t *data2 = new uint32_t[LIMIT + 100];

    printf("Allocating memory: %" PRIu64 " (%.2lf Mbytes) \n", sizeof(uint32_t) * (LIMIT + 100), ((double)sizeof(uint32_t) * (LIMIT + 100)) / (1024 * 1024));

    uint32_t *ptr1 = data1;
    uint32_t *ptr2 = data2;


    while (((uint64_t)(void*)ptr1) % 16 != 0 ) ptr1++;
    while (((uint64_t)(void*)ptr2) % 16 != 8 ) ptr2++;

    /* Make OS commit physical pages for the allocated memory */
    printf("Makeing OS commit physical pages for the allocated memory \n");
    memset(ptr1, 0, sizeof(uint32_t) * LIMIT);
    memset(ptr2, 0, sizeof(uint32_t) * LIMIT);

    for (int g = 0; g < 1; g++)
    {
         Int32x4 value(0xA5A5A5A5);

         uint32_t *ptrAU = ptr1;
         uint32_t *ptrAA = ptr1;
         uint32_t *ptrAS = ptr1;
         uint32_t *ptrRAS = ptr1;
         uint32_t *ptrU  = ptr2;

        start = PreciseTimer::currentTime();
        for (int i=0; i < LIMIT; i += 32)
        {
              value.save(ptrAU     );
              value.save(ptrAU +  4);
              value.save(ptrAU +  8);
              value.save(ptrAU + 12);
              value.save(ptrAU + 16);
              value.save(ptrAU + 20);
              value.save(ptrAU + 24);
              value.save(ptrAU + 28);
              ptrAU += 32;
        }
        delay = start.usecsToNow();
        printf("Delay Aligned Addr UnAlign Acc :%8" PRIu64 "us \n", delay);

        start = PreciseTimer::currentTime();
        for (int i = 0; i < LIMIT; i += 32)
        {
            value.saveAligned(ptrAA     );
            value.saveAligned(ptrAA +  4);
            value.saveAligned(ptrAA +  8);
            value.saveAligned(ptrAA + 12);
            value.saveAligned(ptrAA + 16);
            value.saveAligned(ptrAA + 20);
            value.saveAligned(ptrAA + 24);
            value.saveAligned(ptrAA + 28);
            ptrAA += 32;
        }
        delay = start.usecsToNow();
        printf("Delay Aligned Addr Aligned Acc :%8" PRIu64 "us \n", delay);

        start = PreciseTimer::currentTime();
        for (int i = 0; i < LIMIT; i += 32)
        {
            value.streamAligned(ptrAS);
            value.streamAligned(ptrAS + 4);
            value.streamAligned(ptrAS + 8);
            value.streamAligned(ptrAS + 12);
            value.streamAligned(ptrAS + 16);
            value.streamAligned(ptrAS + 20);
            value.streamAligned(ptrAS + 24);
            value.streamAligned(ptrAS + 28);
            ptrAS += 32;
        }
        delay = start.usecsToNow();
        printf("Delay Aligned Addr Aligned Str :%8" PRIu64 "us \n", delay);

           start = PreciseTimer::currentTime();
        for (int i = 0; i < LIMIT; i += 4)
        {
            value.streamAligned(ptrRAS);
            ptrRAS += 4;
        }
        delay = start.usecsToNow();
        printf("Delay not unrolled Aligned Str :%8" PRIu64 "us \n", delay);


        start = PreciseTimer::currentTime();
        for (int i = 0; i < LIMIT; i += 32)
        {
            value.save(ptrU);
            value.save(ptrU + 4);
            value.save(ptrU + 8);
            value.save(ptrU + 12);
            value.save(ptrU + 16);
            value.save(ptrU + 20);
            value.save(ptrU + 24);
            value.save(ptrU + 28);
            ptrU += 32;
         }
        delay = start.usecsToNow();
        printf("Delay UnAligned Addr           :%8" PRIu64 "us \n", delay);


        start = PreciseTimer::currentTime();
        memset(ptr1, 0xA5, sizeof(uint32_t) * LIMIT);
        delay = start.usecsToNow();
        printf("Memset delay                   :%8" PRIu64 "us \n", delay);

    }

    delete[] data1;
    delete[] data2;
#endif
}

template<template<typename> class Kernel>
void _profileVectorKernel(const char *name, bool vectorRun = 1, const Kernel<DummyAlgebra> &kernel = Kernel<DummyAlgebra>())
{
    const static int inputNumber  = Kernel<DummyAlgebra>::inputNumber;
    const static int outputNumber = Kernel<DummyAlgebra>::outputNumber;


    PreciseTimer start;
    uint64_t delayScalar;



    G12Buffer * input[POLUTING_INPUTS][inputNumber];
    VisiterSemiRandom<> vis;
    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        for (int j = 0; j < inputNumber; j++)
        {
            input[i][j] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
            input[i][j]->touchOperationElementwize(vis);
        }
    }

    G12Buffer *outputScalar[POLUTING_INPUTS][outputNumber];
    G12Buffer *outputVector[POLUTING_INPUTS][outputNumber];

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        for (int j = 0; j < outputNumber; j++)
        {
            outputScalar[i][j] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
            outputVector[i][j] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
        }
    }
    BufferProcessor<G12Buffer, G12Buffer, Kernel, G12BufferAlgebraScalar> procScalar;


    printf("Profiling Scalar Algebra       %5.5s %d.%d:", name, inputNumber, outputNumber);
    start = PreciseTimer::currentTime();
    for (unsigned i = 0; i < LIMIT; i++) {
        procScalar.process(input[i % POLUTING_INPUTS], outputScalar[i % POLUTING_INPUTS], kernel);
    }
    delayScalar = start.usecsToNow();
    printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delayScalar, delayScalar / 1000, delayScalar / LIMIT); fflush(stdout);


    if (vectorRun)
    {
#ifdef WITH_SSE
        uint64_t delayVector;
        BufferProcessor<G12Buffer, G12Buffer, Kernel, G12BufferAlgebra> procVector;

        /* Simple vector processing */
        printf("Profiling SSE Vector Algebra   %5.5s %d.%d:", name, inputNumber, outputNumber);
        start = PreciseTimer::currentTime();
        for (unsigned i = 0; i < LIMIT; i++) {
            procVector.process(input[i % POLUTING_INPUTS], outputVector[i % POLUTING_INPUTS], kernel);
        }
        delayVector = start.usecsToNow();
        printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us %4.2lf%%", delayVector, delayVector / 1000, delayVector / LIMIT, delayVector * 100.0 / delayScalar); fflush(stdout);

        for (int j = 0; j < outputNumber; j++) {
            CORE_ASSERT_TRUE_P(outputScalar[0][j]->isEqual(outputVector[0][j]), ("Scalar and Vector computation results are not equal. Output %d is different\n", j));
        }
        printf ("+\n");

        /* Aligned output */

        printf("Profiling SSE Vector Aligned   %5.5s %d.%d:", name, inputNumber, outputNumber);
        start = PreciseTimer::currentTime();
        for (unsigned i = 0; i < LIMIT; i++) {
            procVector.processSaveAligned(input[i % POLUTING_INPUTS], outputVector[i % POLUTING_INPUTS], kernel);
        }
        delayVector = start.usecsToNow();
        printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us %4.2lf%%", delayVector, delayVector / 1000, delayVector / LIMIT, delayVector * 100.0 / delayScalar); fflush(stdout);

        for (int j = 0; j < outputNumber; j++) {
            CORE_ASSERT_TRUE_P(outputScalar[0][j]->isEqual(outputVector[0][j]), ("Scalar and Vector computation results are not equal. Output %d is different\n", j));
        }
        printf ("+\n");

        /* Streaming */
        BufferProcessor<G12Buffer, G12Buffer, Kernel, G12BufferAlgebraStreaming> procVectorStream;
        printf("Profiling SSE Vector Streaming %5.5s %d.%d:", name, inputNumber, outputNumber);
        start = PreciseTimer::currentTime();
        for (unsigned i = 0; i < LIMIT; i++) {
            procVectorStream.processSaveAligned(input[i % POLUTING_INPUTS], outputVector[i % POLUTING_INPUTS], kernel);
        }
        delayVector = start.usecsToNow();
        printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us %4.2lf%%", delayVector, delayVector / 1000, delayVector / LIMIT, delayVector * 100.0 / delayScalar); fflush(stdout);

        for (int j = 0; j < outputNumber; j++) {
            CORE_ASSERT_TRUE_P(outputScalar[0][j]->isEqual(outputVector[0][j]), ("Scalar and Vector computation results are not equal. Output %d is different\n", j));
        }
        printf ("+\n");
#endif
    }

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
       for (int j = 0; j < inputNumber; j++)
           delete input[i][j];

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
       for (int j = 0; j < outputNumber; j++)
       {
           delete outputScalar[i][j];
           delete outputVector[i][j];
       }
    }
}


void _profileMemcpy()
{
    PreciseTimer start;
    uint64_t delay;
    G12Buffer * input[POLUTING_INPUTS];
    G12Buffer *output[POLUTING_INPUTS];
    VisiterSemiRandom<> vis;
    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
            input[i] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
            output[i] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
            input[i]->touchOperationElementwize(vis);
    }


    printf("Profiling Memcpy               %5.5s %d.%d:", "memcpy", 1,1);
    start = PreciseTimer::currentTime();
    for (unsigned i = 0; i < LIMIT; i++)
    {
        memcpy(
                output[i % POLUTING_INPUTS]->data,
                input[i % POLUTING_INPUTS]->data,
                sizeof(G12Buffer::InternalElementType) * output[i % POLUTING_INPUTS]->stride * output[i % POLUTING_INPUTS]->h);
    }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / LIMIT); fflush(stdout);

    printf("Profiling Memcpy               %5.5s %d.%d:", "lncpy", 1,1);
    start = PreciseTimer::currentTime();
    for (unsigned ii = 0; ii < LIMIT; ii++)
    {
        output[ii % POLUTING_INPUTS]->fillWith(input[ii % POLUTING_INPUTS]);
    }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / LIMIT); fflush(stdout);

}

/**
 *  This function profiles the Sobel Horisontal kernel applied in a reasonably optimized
 *  but not parallelized or vectorized manner.
 *
 **/
void _profileManualSobelH (void)
{
     //int const TEST_H_SIZE = 600;
     //int const TEST_W_SIZE = 1100;
     PreciseTimer start;
     uint64_t delay;

     G12Buffer * input[POLUTING_INPUTS];
     G12Buffer * output[POLUTING_INPUTS];

     VisiterSemiRandom<> vis;
     for (unsigned i = 0; i < POLUTING_INPUTS; i++)
     {
         input[i]  = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
         input[i]->touchOperationElementwize(vis);
         output[i] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
     }
     printf("Profiling Manual Kernel        %5.5s %d.%d:", "Sob H", 1, 1);
     start = PreciseTimer::currentTime();
     for (unsigned k = 0; k < LIMIT; k++) {
         G12Buffer *in  = input[k % POLUTING_INPUTS];
         G12Buffer *out = output[k % POLUTING_INPUTS];
         for (int i = 1; i < in->h - 1; i++)
         {
             uint16_t *start = &(in->element(i - 1, 0));
             uint16_t *startDst = &(out->element(i, 0));

             for (int j = 1; j < in->w - 1; j++)
             {
                 uint16_t a = *(start);
                 uint16_t b = *(start + in->stride);
                 uint16_t c = *(start + 2 * in->stride);

                 uint16_t a1 = *(start + 2);
                 uint16_t b1 = *(start + in->stride + 2);
                 uint16_t c1 = *(start + 2 * in->stride + 2);
                 *startDst = (a + 2 * (b - b1) + c - a1 - c1) >> 3;
                 start++;
                 startDst++;
             }
         }
     }
     delay = start.usecsToNow();
     printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / LIMIT); fflush(stdout);
}

void _profileManualSobelHSimple (void)
{
     PreciseTimer start;
     uint64_t delay;

     G12Buffer * input[POLUTING_INPUTS];
     G12Buffer * output[POLUTING_INPUTS];

     VisiterSemiRandom<> vis;
     for (unsigned i = 0; i < POLUTING_INPUTS; i++)
     {
         input[i]  = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
         input[i]->touchOperationElementwize(vis);
         output[i] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
     }
     printf("Profiling ManualS Kernel       %5.5s %d.%d:", "Sob H", 1, 1);
     start = PreciseTimer::currentTime();
     for (unsigned k = 0; k < LIMIT; k++) {
         G12Buffer *in  = input[k % POLUTING_INPUTS];
         G12Buffer *out = output[k % POLUTING_INPUTS];
         for (int i = 1; i < in->h - 1; i++)
         {
             for (int j = 1; j < in->w - 1; j++)
             {
                 out->element(i,j) =
                         (    in->element(i - 1, j - 1) +
                         2 * in->element(i    , j - 1) +
                             in->element(i + 1, j - 1) -
                             in->element(i - 1, j + 1) -
                         2 * in->element(i    , j + 1) -
                             in->element(i + 1, j + 1)) / 8;
             }
         }
     }
     delay = start.usecsToNow();
     printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / LIMIT); fflush(stdout);
}


void _profileManualEdgeSimple (void)
{
     PreciseTimer start;
     uint64_t delay;

     G12Buffer * input[POLUTING_INPUTS];
     G12Buffer * output[POLUTING_INPUTS];

     VisiterSemiRandom<> vis;
     for (unsigned i = 0; i < POLUTING_INPUTS; i++)
     {
         input[i]  = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
         input[i]->touchOperationElementwize(vis);
         output[i] = new G12Buffer(TEST_H_SIZE ,TEST_W_SIZE);
     }
     printf("Profiling ManualS Kernel       %5.5s %d.%d:", "Edge", 1, 1);
     start = PreciseTimer::currentTime();
     for (unsigned k = 0; k < LIMIT; k++) {
         G12Buffer *in  = input[k % POLUTING_INPUTS];
         G12Buffer *out = output[k % POLUTING_INPUTS];
         for (int i = 0; i < in->h - 2; i++)
         {
             for (int j = 0; j < in->w - 2; j++)
             {
                uint16_t a00 = in->element(i + 0, j + 0);
                uint16_t a01 = in->element(i + 0, j + 1);
                uint16_t a02 = in->element(i + 0, j + 2);
                uint16_t a10 = in->element(i + 1, j + 0);
                uint16_t a12 = in->element(i + 1, j + 2);
                uint16_t a20 = in->element(i + 2, j + 0);
                uint16_t a21 = in->element(i + 2, j + 1);
                uint16_t a22 = in->element(i + 2, j + 2);

                int16_t positiveH = int16_t (a00 + a20 + 2 * a10);
                int16_t negativeH = int16_t (a02 + a22 + 2 * a12);
                int16_t resultH = ( positiveH - negativeH ) / 4;


                int16_t positiveV = int16_t (a00 + a02 + 2 * a01);
                int16_t negativeV = int16_t (a20 + a22 + 2 * a21);
                int16_t resultV = ( positiveV - negativeV ) / 4;

                uint16_t final = std::abs(resultV) + std::abs(resultH);

                out->element(i,j) = final;
             }
         }
    }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delay, delay / 1000, delay / LIMIT); fflush(stdout);
}



TEST(FastKernelProfile, main)
{
    _profileAlgnmentAccessSpeed();
    _profileVectorKernel<Blur5Horisontal>("Hor");
    _profileVectorKernel<Blur5Vertical>("Ver");

    _profileVectorKernel<SobelHorizontalKernel>("Sob H");
    _profileManualSobelH();
    _profileManualSobelHSimple();

    _profileVectorKernel<SobelVerticalKernel>("Sob V");
    _profileVectorKernel<Gaussian3x3Kernel>  ("Ga3x3");
    _profileVectorKernel<Laplacian3x3Kernel> ("La3x3");

    _profileVectorKernel<SumBuffers>("Add");
    _profileVectorKernel<SubtractBuffers>("Sub");
    _profileVectorKernel<MixBuffers>("Fade");
    _profileVectorKernel<DifferenceBuffers>("Diff");
    _profileVectorKernel<ThresholdBinariseKernel>("Thres");

    uint16_t data[9*9] = {
            0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
            0x00, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF,
            0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
            0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF,
            0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
            0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
            0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
            0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF
    };

    G12Buffer *element = new G12Buffer( 9, 9, data);
    _profileVectorKernel<ErodeKernel>("er9x9", 1, ErodeKernel<DummyAlgebra>(element, 4, 4));
    _profileVectorKernel<DilateKernel>("dl9x9", 1, DilateKernel<DummyAlgebra>(element, 4, 4));
    delete element;

    _profileVectorKernel<EdgeMagnitude>("Edge");
    _profileManualEdgeSimple();
    _profileVectorKernel<CopyKernel>("Copy");
    _profileMemcpy();
}
