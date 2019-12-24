/**
 * \file main_test_matrix_profile.cpp
 * \brief This is the main file for the test matrix
 *
 * \date Oct 07, 2015
 * \author sfed
 *
 * \ingroup perf-tests
 */

#include <sstream>
#include <iostream>
#include <limits>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/matrix/matrix.h"
#include "core/utils/preciseTimer.h"

using namespace corecvs;

const static unsigned POLUTING_INPUTS = 20;
const static unsigned LIMIT = 5;


TEST(MatrixProfile, testMul1000)
{
    PreciseTimer start;
    Matrix * input[POLUTING_INPUTS];

    const static int  TEST_H_SIZE = 1000;
    const static int  TEST_W_SIZE = TEST_H_SIZE;

    for (unsigned i = 0; i < POLUTING_INPUTS; i++)
    {
        input[i] = new Matrix(TEST_H_SIZE ,TEST_W_SIZE);
        auto touch = [](int i, int j, double &el) -> void { el = ((i+1) * (j + 1)) + ((j + 1) / 5.0); };
        input[i]->touchOperationElementwize(touch);
    }

    SYNC_PRINT(("Profiling     Simple Approach:"));
    start = PreciseTimer::currentTime();
    for (unsigned i = 0; i < LIMIT; i++) {
        Matrix &A = *input[i % POLUTING_INPUTS];
        Matrix B = A * A;
    }
    uint64_t delaySimple = start.usecsToNow();
    SYNC_PRINT(("%8" PRIu64 "us %8" PRIu64 "ms SP: %8" PRIu64 "us\n", delaySimple, delaySimple / 1000, delaySimple / LIMIT));

    for (unsigned i = 0; i < POLUTING_INPUTS; i++) {
        delete_safe(input[i]);
    }
}

void printHeader()
{
    SYNC_PRINT(("|           |      Test size, mem and algo       | Runs|"));
    SYNC_PRINT(("   Time us   |   Time ms  |    One run us  |    Throughput    | \n"));
}

void printName(const char *name, int testsize, double mem, int runs)
{
    SYNC_PRINT(("| Profiling | [%5dx%-5d] (%6.1lf Mb) %8s | %3d |", testsize, testsize, mem / 1000000.0, name, runs));
}

void printResult(double gflops, uint64_t delay, int runs)
{
    double runss = 1000000.0 / ((double)delay / runs);
    double gflopss = runss * gflops;

    SYNC_PRINT(("%10" PRIu64 "us | %8" PRIu64 "ms | SP: %8" PRIu64 "us | % 7.3lf Gflops/s |\n",
            delay,
            delay / 1000,
            delay / runs,
            gflopss
               ));
}

TEST(MatrixProfile, testMulSize3)
{
//    int  sizes    [] = { 1024, 2048, 4096, 16384 };

    int  sizes    [] = { 1000, 2000, 4000 };

    int  polca    [] = {   10,   20,    5,     1 };
    int  runs     [] = {   10,    5,    2,     2 };

    bool runsimple[] = { true, false, false, false };
    bool runslow  [] = { true, true , false, false };
    bool runour   [] = { true, true ,  true, false };
    bool runfast  [] = { true, true ,  true, false };   // 16K * 16K - skip at all


    printHeader();

    for (size_t testnum = 0; testnum < CORE_COUNT_OF(sizes); testnum++)
    {
        int       TEST_H_SIZE     = sizes[testnum] /* /128*/;
        int       TEST_W_SIZE     = TEST_H_SIZE;
        unsigned  POLUTING_INPUTS = polca[testnum];
        unsigned  LIMIT           = runs[testnum];

        double mem    = 2.0 * sizeof(double) * (double)TEST_H_SIZE * TEST_H_SIZE;
        double flop   = 2.0 * (double)TEST_H_SIZE * TEST_H_SIZE * TEST_H_SIZE;
        double gflop  = flop / 1000000.0 / 1000.0;


        PreciseTimer start;
        Matrix ** input1 = new Matrix*[POLUTING_INPUTS]; // Unfortunately VS2013 does not support C99
        Matrix ** input2 = new Matrix*[POLUTING_INPUTS];

        Matrix AB(1,1);

        for (unsigned i = 0; i < POLUTING_INPUTS; i++)
        {
            input1[i] = new Matrix(TEST_H_SIZE ,TEST_W_SIZE);
            input2[i] = new Matrix(TEST_H_SIZE ,TEST_W_SIZE);

            // auto touch1 = [](int i, int j, double &el) -> void { el = ((i+1) * (j + 1)) + ((j + 1) / 5.0); };
            auto touch1 = [](int i, int j, double &el) -> void
            {
                uint16_t semirand = (uint16_t )(i * 1237657 + j * 235453);
                el = ((double)semirand - 32768) / 65536.0;
            };
            input1[i]->touchOperationElementwize(touch1);

            // auto touch2 = [](int i, int j, double &el) -> void { el = ((i+4) * (j + 1)) + ((i + 1) / 5.0); };
            auto touch2 = [](int i, int j, double &el) -> void
            {
                uint16_t semirand = (uint16_t )(i * 54657 + j * 2517);
                el = ((double)semirand - 32768) / 65536.0;
            };
            input2[i]->touchOperationElementwize(touch2);
        }

        if (runsimple[testnum])
        {
            printName("Simple", TEST_H_SIZE, mem, LIMIT);
            start = PreciseTimer::currentTime();
            for (unsigned i = 0; i < LIMIT; i++) {
                Matrix &A = *input1[i % POLUTING_INPUTS];
                Matrix &B = *input2[i % POLUTING_INPUTS];
                AB = Matrix::multiplyHomebrew(A, B, false, false);
            }
            uint64_t delaySimple = start.usecsToNow();
            printResult(gflop, delaySimple, LIMIT);
        }

        /*if (!AB.isFinite()) {
            SYNC_PRINT(("Matrix is not finite\n"));
        } else {
            SYNC_PRINT(("Matrix is finite - ok\n"));
        }*/


        if (runslow[testnum])
        {
            printName("TBB", TEST_H_SIZE, mem, LIMIT);
            start = PreciseTimer::currentTime();
            for (unsigned i = 0; i < LIMIT; i++) {
                Matrix &A = *input1[i % POLUTING_INPUTS];
                Matrix &B = *input2[i % POLUTING_INPUTS];
                AB = Matrix::multiplyHomebrew(A, B, true, false);
            }
            uint64_t delayTBB = start.usecsToNow();
            printResult(gflop, delayTBB, LIMIT);

#ifdef WITH_SSE
#ifdef WITH_AVX
            printName("AVX/SSE", TEST_H_SIZE, mem, LIMIT);
#else
            printName("---/SSE", TEST_H_SIZE, mem, LIMIT);
#endif
            start = PreciseTimer::currentTime();
            for (unsigned i = 0; i < LIMIT; i++) {
                Matrix &A = *input1[i % POLUTING_INPUTS];
                Matrix &B = *input2[i % POLUTING_INPUTS];
                AB = Matrix::multiplyHomebrew(A, B, false, true);
            }
            uint64_t delayVector = start.usecsToNow();
            printResult(gflop, delayVector, LIMIT);
#endif
        }

        if (runour[testnum])
        {
            printName("All On", TEST_H_SIZE, mem, LIMIT);
            start = PreciseTimer::currentTime();
            for (unsigned i = 0; i < LIMIT; i++) {
                Matrix &A = *input1[i % POLUTING_INPUTS];
                Matrix &B = *input2[i % POLUTING_INPUTS];
                AB = Matrix::multiplyHomebrew(A, B, true, true);
            }
            uint64_t delayHome = start.usecsToNow();
            printResult(gflop, delayHome, LIMIT);

        }

#ifdef WITH_AVX
        if (runour[testnum])
        {
            printName("BlockMM8", TEST_H_SIZE, mem, LIMIT);
            start = PreciseTimer::currentTime();
            for (unsigned i = 0; i < LIMIT; i++) {
                Matrix &A = *input1[i % POLUTING_INPUTS];
                Matrix &B = *input2[i % POLUTING_INPUTS];
                AB = Matrix::multiplyBlasReplacement(A, B);
            }
            uint64_t delayRepl = start.usecsToNow();
            printResult(gflop, delayRepl, LIMIT);
        }
#endif

        if (runfast[testnum]) {
#ifdef WITH_BLAS
            printName("OpenBLAS", TEST_H_SIZE, mem, LIMIT);
            start = PreciseTimer::currentTime();
            for (unsigned i = 0; i < LIMIT; i++) {
                Matrix &A = *input1[i % POLUTING_INPUTS];
                Matrix &B = *input2[i % POLUTING_INPUTS];
                AB = Matrix::multiplyBlas(A, B);
            }
            uint64_t delayBlas = start.usecsToNow();
            printResult(gflop, delayBlas, LIMIT);
#endif // WITH_BLAS
        }

        for (unsigned i = 0; i < POLUTING_INPUTS; i++) {
            delete_safe(input1[i]);
            delete_safe(input2[i]);
        }
        delete[] input1;
        delete[] input2;
    }
}
