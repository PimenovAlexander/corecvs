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

#ifndef ASSERTS
#define ASSERTS
#endif

#include "global.h"
#include "mathUtils.h"
#include "ellipticalApproximation.h"


using namespace std;
using namespace corecvs;

void testMoments1(void)
{
    cout << "Running testMoments1" << endl;
    EllipticalApproximation ell;
    ell.addPoint(Vector2dd(0.0,0.0));
    ell.addPoint(Vector2dd(1.0,1.0));

    ASSERT_DOUBLE_EQUAL(ell.getCenter().x(), 0.5, "Wrong center");
    ASSERT_DOUBLE_EQUAL(ell.getCenter().y(), 0.5, "Wrong center");

    double angle = ell.getEllipseOrientation();
    Vector2dd axis = ell.getEllipseAxis();
    printf("angle %2.2lf\n", radToDeg(angle));
    printf("axis %2.2lf %2.2lf\n", axis.x(), axis.y());
    ASSERT_DOUBLE_EQUAL(radToDeg(angle), 45.0, "Wrong orientation");

    Vector2dd mainDir = ell.getMainDirection();

    cout << "Main direction:" << mainDir << endl;
    cout << "Main direction1:" << Vector2dd::FromPolar(angle, 1.0) << endl;
}

void testMoments2(void)
{
    cout << "Running testMoments2" << endl;
    EllipticalApproximation ell;
    ell.addPoint(Vector2dd(0.0,0.0));
    ell.addPoint(Vector2dd(0.0,1.0));

    ASSERT_DOUBLE_EQUAL(ell.getCenter().x(),0.0, "Wrong center");
    ASSERT_DOUBLE_EQUAL(ell.getCenter().y(),0.5, "Wrong center");

    double angle = ell.getEllipseOrientation();
    Vector2dd axis = ell.getEllipseAxis();
    printf("angle %2.2lf\n", angle / M_PI * 180);
    printf("axis %2.2lf %2.2lf\n", axis.x(), axis.y());
    ASSERT_DOUBLE_EQUAL(angle / M_PI * 180, 90.0, "Wrong orientation");
}

void testMoments3(void)
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


int main (int /*argC*/, char ** /*argV*/)
{
    testMoments1();
    testMoments2();
    testMoments3();
    cout << "PASSED" << endl;
    return 0;
}
