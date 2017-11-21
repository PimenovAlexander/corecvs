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
#include "core/math/quaternion.h"
#include "core/utils/preciseTimer.h"

using namespace corecvs;


TEST(Affine, profileHamilton)
{
    static unsigned LIMIT = 10000;
    static unsigned INPUTS = 1000;

    vector<Quaternion> muls(INPUTS);

    printf("Testing unitary mul\n");
    for (unsigned i = 0; i < INPUTS; i++)
    {
        Vector3dd x(
                (rand() % 4000) - 2000,
                (rand() % 4000) - 2000,
                (rand() % 4000) - 2000);
        if (!x == 0 ) x = Vector3dd(1.0);
        x = x / !x;
        muls[i] = Quaternion(x, ((rand() % 4000) / 2000.0 * M_PI));
    }

    PreciseTimer timer;
    uint64_t delay;

    timer = PreciseTimer::currentTime();
    Quaternion a;

    a = Quaternion(0.5, 0.5, 0.5, 0.5);
    for (unsigned i = 0; i < LIMIT; i++)
    {
        a = a / !a;
        for (unsigned j = 0; j < INPUTS; j++)
        {
            a = a ^ muls[j];
        }
    }
    delay = timer.usecsToNow();
    printf("Classic   : %8" PRIu64 "us\n", delay); fflush(stdout);
    std::cout << a << std::endl;

    timer = PreciseTimer::currentTime();
    a = Quaternion(0.5, 0.5, 0.5, 0.5);
    for (unsigned i = 0; i < LIMIT; i++)
    {
        a = a / !a;
        for (unsigned j = 0; j < INPUTS; j++)
        {
            a = hamilton1(a, muls[j]);
        }
    }
    delay = timer.usecsToNow();
    printf("Additions: %8" PRIu64 "us\n", delay);
    fflush(stdout);
    std::cout << a << std::endl;
}
