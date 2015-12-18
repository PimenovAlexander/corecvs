/**
 * \file main_test_linear.cpp
 * \brief This is the main file for the test linear 
 *
 * \date Oct 01, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"

#include "line.h"


using namespace std;
using namespace corecvs;


TEST(Linear, testPlane2Point)
{
    Plane3d p = Plane3d::FromPoints(
            Vector3dd(1.0, 0.0, 0.0),
            Vector3dd(0.0, 1.0, 0.0),
            Vector3dd(0.0, 0.0, 1.0)
    );

    Vector3dd point(0.0);

    CORE_ASSERT_DOUBLE_EQUAL(p.distanceTo(point), sqrt(1.0/3), "Wrong plane to point distance");

    Vector3dd proj = p.projectPointTo(point);
    cout << "Projection: " << proj << endl;
    Vector3dd rproj = Vector3dd(1.0,1.0,1.0) / 3.0;
    cout << "Expected:   " << rproj << endl;

    CORE_ASSERT_TRUE(proj.notTooFar(rproj, 1e-7), "Wrong projection");
}


TEST(Linear, testPlane2Plane)
{
    Plane3d diagonal = Plane3d::FromPoints(
            Vector3dd(1.0, 0.0, 0.0),
            Vector3dd(0.0, 1.0, 0.0),
            Vector3dd(0.0, 0.0, 1.0)
    );

    Plane3d vertical = Plane3d::FromPoints(
            Vector3dd(1.0, 0.0, 0.0),
            Vector3dd(0.0, 0.0, 0.0),
            Vector3dd(0.0, 0.0, 1.0)
    );

    Ray3d ray = diagonal.intersectWith(vertical);

    cout << "Ray:" << endl;
    cout << ray << endl;

    Vector3dd p1 = ray.getPoint(0.0);
    Vector3dd p2 = ray.getPoint(1.0);

    cout << "Deviations:" << endl;
    cout << diagonal.deviationTo(p1) << endl;
    cout << diagonal.deviationTo(p2) << endl;
    cout << vertical.deviationTo(p1) << endl;
    cout << vertical.deviationTo(p2) << endl;

    CORE_ASSERT_DOUBLE_EQUAL(diagonal.deviationTo(p1), 0.0, "Wrong plane intersection1");
    CORE_ASSERT_DOUBLE_EQUAL(diagonal.deviationTo(p2), 0.0, "Wrong plane intersection2");


    CORE_ASSERT_DOUBLE_EQUAL(vertical.deviationTo(p1), 0.0, "Wrong plane intersection3");
    CORE_ASSERT_DOUBLE_EQUAL(vertical.deviationTo(p2), 0.0, "Wrong plane intersection4");
}

TEST(Linear, testRay2RayDistance)
{
    Ray3d ray1(Vector3dd(0.0, 0.0, 1.0), Vector3dd(0.0, 0.0, 0.0));
    Ray3d ray2(Vector3dd(1.0, 0.0, 0.0), Vector3dd(0.0, 1.0, 0.0));

    double result = ray1.distanceTo(ray2);
    CORE_ASSERT_DOUBLE_EQUAL(result,1.0, "Wrong ray to ray distance");
}

TEST(Linear, testRay2RayIn2d)
{
    /* Two square diagonals*/
    Ray2d ra(Vector2dd(2.0,  1.0), Vector2dd(0.0, 0.0));
    Ray2d rb(Vector2dd(2.0, -1.0), Vector2dd(0.0, 1.0));

    Line2d la(ra);
    Line2d lb(rb);

    ASSERT_TRUE(la.distanceTo(ra.getPoint(0.0)) < 1e-10);
    ASSERT_TRUE(la.distanceTo(ra.getPoint(1.0)) < 1e-10);

    ASSERT_TRUE(lb.distanceTo(rb.getPoint(0.0)) < 1e-10);
    ASSERT_TRUE(lb.distanceTo(rb.getPoint(1.0)) < 1e-10);

    bool hasIntersection = false;
    Vector2dd point = la.intersectWith(lb, &hasIntersection);

    ASSERT_TRUE(hasIntersection);
    //cout << "Intersection" << std::endl;
    //cout << point << std::endl;

    ASSERT_TRUE(la.distanceTo(point) < 1e-10);
    ASSERT_TRUE(lb.distanceTo(point) < 1e-10);

    ASSERT_TRUE(point.notTooFar(Vector2dd(1, 0.5), 1e-10));
}

TEST(Linear, testSegment2LineIn2d)
{
    Vector2dd p1(3.0,  4.0);
    Vector2dd p2(4.0, -9.0);

    Segment2d s(p1, p2);
    Ray2d r(s);
    Line2d l(s);

    cout << "P1: " << p1 << endl;
    cout << "P2: " << p2 << endl;

    cout << "S :" << s << endl;
    cout << "R :" << r << endl;
    cout << "L :" << l << endl;



    ASSERT_TRUE(l.distanceTo(p1) < 1e-10);
    ASSERT_TRUE(l.distanceTo(p2) < 1e-10);
}

#if 0
int main (int /*argC*/, char ** /*argV*/)
{
    testPlane2Point();
    testRay2Ray();
    testPlane2Plane();
    cout << "PASSED" << endl;
    return 0;
}
#endif
