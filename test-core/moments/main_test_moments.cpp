/**
 * \file main_test_moments.cpp
 * \brief This is the main file for the test moments 
 *
 * \date Apr 14, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <core/geometry/mesh/mesh3d.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/math/mathUtils.h"
#include "core/geometry/ellipticalApproximation.h"

using namespace corecvs;

TEST(Moments, testRotations)
{
    cout << "Running testMoments1" << endl;
    EllipticalApproximation ell;
    ell.addPoint(Vector2dd(0.0,0.0));
    ell.addPoint(Vector2dd(1.0,1.0));

    CORE_ASSERT_DOUBLE_EQUAL(ell.getCenter().x(), 0.5, "Wrong center");
    CORE_ASSERT_DOUBLE_EQUAL(ell.getCenter().y(), 0.5, "Wrong center");

    double angle = ell.getEllipseOrientation();
    Vector2dd axis = ell.getEllipseAxis();
    printf("angle %2.2lf\n", radToDeg(angle));
    printf("axis %2.2lf %2.2lf\n", axis.x(), axis.y());
    CORE_ASSERT_DOUBLE_EQUAL(radToDeg(angle), 45.0, "Wrong orientation");

    Vector2dd mainDir = ell.getMainDirection();

    cout << "Main direction:" << mainDir << endl;
    cout << "Main direction1:" << Vector2dd::FromPolar(angle, 1.0) << endl;
}

TEST(Moments, testMoments2)
{
    cout << "Running testMoments2" << endl;
    EllipticalApproximation ell;
    ell.addPoint(Vector2dd(0.0,0.0));
    ell.addPoint(Vector2dd(0.0,1.0));

    CORE_ASSERT_DOUBLE_EQUAL(ell.getCenter().x(), 0.0, "Wrong center");
    CORE_ASSERT_DOUBLE_EQUAL(ell.getCenter().y(), 0.5, "Wrong center");

    double angle = ell.getEllipseOrientation();
    Vector2dd axis = ell.getEllipseAxis();
    printf("angle %2.2lf\n", angle / M_PI * 180);
    printf("axis %2.2lf %2.2lf\n", axis.x(), axis.y());
    CORE_ASSERT_DOUBLE_EQUAL(angle / M_PI * 180, 90.0, "Wrong orientation");
}

TEST(Moments, testMoments3)
{
    cout << "Running testMoments3" << endl;
    EllipticalApproximation ell;
    Vector2dd dir(0.24, 0.56);

    ell.addPoint(0 * dir);
    ell.addPoint(1 * dir);
    ell.addPoint(2 * dir);
    ell.addPoint(3 * dir);
    ell.addPoint(4 * dir);
    ell.addPoint(5 * dir);

    double angle = ell.getEllipseOrientation();
    Vector2dd axis = ell.getEllipseAxis();
    printf("angle %2.2lf\n", radToDeg(angle));
    printf("axis %2.2lf %2.2lf\n", axis.x(), axis.y());
    Vector2dd mainDir = ell.getMainDirection();

    cout << "Main direction :" << mainDir << endl;
    cout << "Main direction1:" << Vector2dd::FromPolar(angle, 1.0) << endl;
    cout << "Main direction2:" << dir.normalised() << endl;
}

TEST(Moments, testMoments4)
{
    cout << "Running testMoments4" << endl;
    EllipticalApproximation1d test;
    test = EllipticalApproximation1d();
    test.addPoint(1.0);
    cout << test.getRadiusAround0()  << endl;
    cout << test.getMean()  << endl;
    cout << test.getMax()  << endl;

    EllipticalApproximation1d test2;
    test2 = test;
    test2.addPoint(0.5);
    cout << test2.getRadiusAround0()  << endl;
    cout << test2.getMean()  << endl;
    cout << test2.getMax()  << endl;

    cout << "passed" << endl;

}

TEST(Moments, testTransform)
{
   Mesh3D mesh;
   mesh.addCylinder(Vector3dd::Zero(), 1, 10);
   mesh.transform((Matrix44)Affine3DQ::RotationX(degToRad(45)));

   EllipticalApproximation3d approx;
   approx.addPoints(mesh.vertexes);
   approx.getEllipseParameters();

   Matrix44 mat  = approx.getEigenTransform();
   Affine3DQ aff = approx.getEigenMove();

   aff.prettyPrint1();

   Matrix44 mat2 = (Matrix44)aff;
   cout << "M1\n" << mat << endl;
   cout << "M2\n" << mat2 << endl;

   cout << "Transform" << aff << endl;

   CORE_ASSERT_TRUE(aff.shift.notTooFar(Vector3dd::Zero(), 1e-7), "Wrong Shift");
   Vector3dd axis = aff.rotor.getAxis();
   double angle   = aff.rotor.getAngle();

   cout << "Axis :" << axis  << endl;
   cout << "Angle:" << radToDeg(angle) << endl;

//   cout << "diff" << (mat / mat2) << endl;


//   CORE_ASSERT_TRUE(mat.notTooFar(mat2, 1e-7), "Wrong move" );

}
