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
//#define TRACE
#endif

#include <vector>
#include <stdint.h>
#include <iostream>
#include <core/cameracalibration/calibrationLocation.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/vector/vector3d.h"
#include "core/math/affine.h"
#include "core/math/mathUtils.h"
#include "core/math/eulerAngles.h"

using namespace corecvs;
using namespace std;


TEST(Affine, testRotations)
{
    Vector3dd vx(1.0,0.0,0.0);
    Affine3DM  aym = Affine3DM::RotationY(degToRad(135.0));
    Affine3DQ  ayq = Affine3DQ::RotationY(degToRad(135.0));
    Quaternion byq = Quaternion::RotationY(degToRad(135.0));
    Quaternion cyq = Quaternion::Rotation(Vector3dd(0.0,1.0,0.0), degToRad(135.0));


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

TEST(Affine, DISABLED_testCoordinateMirror)
{
    Quaternion xr = Quaternion::RotationX(0.3);
    Quaternion yr = Quaternion::RotationY(0.3);
    Quaternion zr = Quaternion::RotationZ(0.3);

    Quaternion tr = Quaternion::RotationZ(0.3) ^ Quaternion::RotationY(0.2) ^ Quaternion::RotationX(0.4);


    /*Vector3dd i1a = Vector3dd(2,3,4);
    Vector3dd i1b = i1a.yxz();

    Quaternion xr1 = Quaternion::RotationX(0.3);
    Quaternion yr1 = Quaternion::RotationX(0.3);
    Quaternion zr1 = Quaternion::RotationX(0.3);


    cout << "Orig" << (xr * i1a) << std::endl;
    cout << "Orig" << (xr * i1b) << std::endl;*/

    Matrix33 T = Matrix33::SwapXY();
    cout << "Orig : " << xr << std::endl;
    cout << "Trans: " <<  Quaternion::FromMatrix(T * xr.toMatrix() * T.inv()) << std::endl;

    cout << "Orig : " << yr << std::endl;
    cout << "Trans: " <<  Quaternion::FromMatrix(T * yr.toMatrix() * T.inv()) << std::endl;

    cout << "Orig : " << zr << std::endl;
    cout << "Trans: " <<  Quaternion::FromMatrix(T * zr.toMatrix() * T.inv()) << std::endl;

    cout << "Orig : " << tr << std::endl;
    cout << "Trans: " <<  Quaternion::FromMatrix(T * tr.toMatrix() * T.inv()) << std::endl;



    //CORE_ASSERT_TRUE_P((xr * i1a).notTooFar( xr1 * i1b , 1e-8), (" Y rotation returned a mistake with Matrix Affine"));
}

TEST(Affine, QuaternionFromMatix)
{
    {
        Matrix33 m = Matrix33::RotationZ(degToRad(45));
        Quaternion q = Quaternion::FromMatrix(m);
        q.printAxisAndAngle();
    }

    {
        Matrix33 m(
         1.07789e-19, -0.204477   ,  0.978871,
        -2.69472e-19,  0.978871   ,  0.204477,
         1          ,  2.85818e-19, -5.04103e-20);

        Quaternion q = Quaternion::FromMatrix(m);
        q.printAxisAndAngle();
    }

    {
        Matrix33 m(
                   0, -0.204477   ,  0.978871,
                   0,  0.978871   ,  0.204477,
                   1,            0,         0);

        Quaternion q = Quaternion::FromMatrix(m);
        q.printAxisAndAngle();
    }
}


TEST(Affine, foo)
{
    auto Q = Quaternion::FromMatrix(corecvs::Matrix33(0, -1, 0, 0, 0, -1, 1, 0, 0));
    std::cout << Q.toMatrix() << std::endl;
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
        std::cout << "Case " << i << std::endl;
        std::cout << Q << " l= " << Q.l2Metric() << std::endl;
        //cout << M << endl;
        std::cout << Q1 << " l= " << Q1.l2Metric() << std::endl;
        ASSERT_TRUE(Q.notTooFar(Q1, 1e-7));
    }

}

TEST(Affine, testEulerAngles)
{
    CameraLocationAngles anglesCam(0.4, 0.7, 0.1);

    Matrix33 matrixCam = anglesCam.toMatrix();
    Quaternion quatCam = Quaternion::FromMatrix(matrixCam);

    CameraLocationAngles anglesCam1 = CameraLocationAngles::FromQuaternion(quatCam);


    std::cout << "A:("  << anglesCam.pitch() << ", "
                        << anglesCam.yaw()   << ", "
                        << anglesCam.roll() << ")" << std::endl;

    std::cout << "M:"   << std::endl << matrixCam << std::endl;
    std::cout << "Q:"   << std::endl << quatCam << std::endl;

    std::cout << "A:("  << anglesCam1.pitch() << ", "
                        << anglesCam1.yaw()   << ", "
                        << anglesCam1.roll() << ")" << std::endl;
}


TEST(Affine, testQuaternionPower)
{
    Quaternion base = Quaternion::Rotation(Vector3dd(1,2,3), degToRad(45));

    cout << "Base     " << base << endl;

    Quaternion baseH = Quaternion::pow(base, 0.5);
    cout << "Base^0.5:" << baseH << endl;
    cout << "Result  :"   << (baseH ^ baseH) << endl;
    cout << endl;

    CORE_ASSERT_TRUE_P((baseH ^ baseH).notTooFar(base, 1e-10), ("Failed with 0.5 power"));

    Quaternion base3 = Quaternion::pow(base, 3);
    cout << "Base^3  : " << (base ^ base ^ base) << endl;
    cout << "Expected: " << base3 << endl;
    cout << endl;

    CORE_ASSERT_TRUE_P((base ^ base ^ base).notTooFar(base3, 1e-10), ("Failed with power 3"));
}
