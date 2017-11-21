/**
 * \file main_test_affine.cpp
 * \brief This is the main file for the test affine 
 *
 * \date Apr 24, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */
#ifndef TRACE
#define TRACE
#endif

#include <vector>
#include <stdint.h>
#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/vector/vector3d.h"
#include "core/math/affine.h"
#include "core/math/mathUtils.h"
#include "core/utils/preciseTimer.h"
#include "core/math/eulerAngles.h"

using namespace std;
using namespace corecvs;

TEST(Affine, testRotations)
{
    Vector3dd vx(1.0,0.0,0.0);
    Affine3DM  aym = Affine3DM::RotationY(degToRad(135.0));
    Affine3DQ  ayq = Affine3DQ::RotationY(degToRad(135.0));
    Quaternion byq = Quaternion::RotationY(degToRad(135.0));
    Quaternion cyq = Quaternion(Vector3dd(0.0,1.0,0.0), degToRad(135.0));


    Vector3dd t1m = aym * vx;
    Vector3dd t1q = ayq * vx;
    Vector3dd t2q = byq * vx;
    Vector3dd t3q = cyq * vx;

    Vector3dd r1 = Vector3dd(-1.0 / sqrt(2.0), 0, -1.0 / sqrt(2.0));
    CORE_ASSERT_TRUE_P(t1m.notTooFar(r1, 1e-8), (" Y rotation returned a mistake with Matrix Affine"));
    CORE_ASSERT_TRUE_P(t1q.notTooFar(r1, 1e-8), (" Y rotation returned a mistake with Quaternion Affine"));
    CORE_ASSERT_TRUE_P(t2q.notTooFar(r1, 1e-8), (" Y rotation returned a mistake with Clean Quaternion"));
    CORE_ASSERT_TRUE_P(t3q.notTooFar(r1, 1e-8), (" Y rotation returned a mistake with Clean Quaternion"));
}

TEST(Affine, testMatrixToQuaternion)
{
    const int TEST_SIZE = 9;
    vector<Vector3dd> axis(TEST_SIZE);
    double angle[TEST_SIZE];

    angle[0] = 0.01;
    axis [0] = Vector3dd(1.0, 1.0, 1.0);
    angle[1] = 0.01;
    axis [1] = Vector3dd(1.0, 0.0, 0.0);
    angle[2] = 0.01;
    axis [2] = Vector3dd(0.0, 1.0, 0.0);
    angle[3] = 0.01;
    axis [3] = Vector3dd(0.0, 0.0, 1.0);
    angle[4] = 0.7;
    axis [4] = Vector3dd(0.0, 1.0, 1.0);
    angle[5] = -0.01;
    axis [5] = Vector3dd(1.0, 0.0, 0.0);
    angle[6] = -0.01;
    axis [6] = Vector3dd(0.0, 1.0, 0.0);
    angle[7] = -0.01;
    axis [7] = Vector3dd(0.0, 0.0, 1.0);
    angle[8] = -0.7;
    axis [8] = Vector3dd(0.0, 1.0, 1.0);


    for (int i = 0; i < TEST_SIZE; i++)
    {
        Quaternion Q = Quaternion::Rotation(axis[i], angle[i]);
        Matrix33 M = Q.toMatrix();
        Quaternion Q1 = Quaternion::FromMatrix(M);
        cout << "Case " << i << endl;
        cout << Q  << " l= " << Q .l2Metric() << endl;
        //cout << M << endl;
        cout << Q1 << " l= " << Q1.l2Metric() << endl;
        ASSERT_TRUE(Q.notTooFar(Q1, 1e-7));
    }

}

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
    cout << a << endl;

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
    printf("Additions: %8" PRIu64 "us\n", delay); fflush(stdout);
    cout << a << endl;
}

TEST(Affine, testEulerAngles)
{
    CameraAnglesLegacy anglesCam(0.7, 0.4, 0.1);
    Matrix33 matrixCam = anglesCam.toMatrix();
    Quaternion quatCam = Quaternion::FromMatrix(matrixCam);
    CameraAnglesLegacy anglesCam1 = CameraAnglesLegacy::FromQuaternion(quatCam);


    cout << "A:(" << anglesCam.pitch() << ", "
                  << anglesCam.yaw()   << ", "
                  << anglesCam.roll()  << ")" << endl;

    cout << "M:" << endl << matrixCam << endl;
    cout << "Q:" << endl << quatCam << endl;

    cout << "A:(" << anglesCam1.pitch() << ", "
                  << anglesCam1.yaw()   << ", "
                  << anglesCam1.roll()  << ")" << endl;
}
