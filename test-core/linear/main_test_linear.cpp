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

#include "core/utils/global.h"

#include "core/geometry/line.h"

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
    std::cout << "Projection: " << proj << std::endl;
    Vector3dd rproj = Vector3dd(1.0,1.0,1.0) / 3.0;
    std::cout << "Expected:   " << rproj << std::endl;

    CORE_ASSERT_TRUE(proj.notTooFar(rproj, 1e-7), "Wrong projection");
}

TEST(Linear, testPlaneAndRay)
{
    {
         Plane3d plane(Vector3dd::OrtY(), -10);

         Ray3d ray(Vector3dd(0.0, -1.0, 1.0), Vector3dd::Zero());

         Vector3dd point = plane.intersectWith(ray);

         std::cout << "Point:" << std::endl;
         std::cout << point << std::endl;
         CORE_ASSERT_TRUE(point.notTooFar(Vector3dd(0, 10, -10)), "Wrong intersection");
         CORE_ASSERT_TRUE(plane.distanceTo(point) < 1e-3, "Wrong intersection");

         double t = plane.intersectWithP(ray);
         std::cout << "T:" << std::endl;
         std::cout << t << std::endl;

         std::cout << "P(t):" << std::endl;
         std::cout << ray.getPoint(t) << std::endl;
    }

     /**/
    {
         Ray3d ray1;
         ray1.p = Vector3dd(23.1231, -26.9769, 114.821);
         ray1.a = Vector3dd(-143.123, -43.0231, -64.8209);
         ray1.a.normalise();

         Plane3d plane1(Vector3dd::OrtY(), 25.0);

         Vector3dd point = plane1.intersectWith(ray1);

         std::cout << "Point:" << std::endl;
         std::cout << point << std::endl;
         CORE_ASSERT_TRUE(plane1.distanceTo(point) < 1e-3, "Wrong intersection");

         double t = plane1.intersectWithP(ray1);
         std::cout << "T:" << std::endl;
         std::cout << t << std::endl;

         std::cout << "P(t):" << std::endl;
         std::cout << ray1.getPoint(t) << std::endl;
         CORE_ASSERT_TRUE(plane1.distanceTo(ray1.getPoint(t)) < 1e-3, "Wrong intersection");

/*
         double denum = plane1.normal() & ray1.a;
         double num   = ((plane1.normal() & ray1.p) + plane1.last());
         std::cout << "Plane normal " << plane1.normal() << std::endl;
         std::cout << "Plane last   " << plane1.last() << std::endl;


         std::cout << "Denum " << denum << std::endl;
         std::cout << "Num " << num << std::endl;
         std::cout << "T1 " << num / denum << std::endl;
*/
    }


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

    std::cout << "Ray:" << std::endl;
    std::cout << ray << std::endl;

    Vector3dd p1 = ray.getPoint(0.0);
    Vector3dd p2 = ray.getPoint(1.0);

    std::cout << "Deviations:" << std::endl;
    std::cout << diagonal.deviationTo(p1) << std::endl;
    std::cout << diagonal.deviationTo(p2) << std::endl;
    std::cout << vertical.deviationTo(p1) << std::endl;
    std::cout << vertical.deviationTo(p2) << std::endl;

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

    std::cout << "P1: " << p1 << std::endl;
    std::cout << "P2: " << p2 << std::endl;

    std::cout << "S :" << s << std::endl;
    std::cout << "R :" << r << std::endl;
    std::cout << "L :" << l << std::endl;

    ASSERT_TRUE(l.distanceTo(p1) < 1e-10);
    ASSERT_TRUE(l.distanceTo(p2) < 1e-10);
}

#if 0
int main (int /*argC*/, char ** /*argV*/)
{
    testPlane2Point();
    testRay2Ray();
    testPlane2Plane();
    std::cout << "PASSED" << std::endl;
    return 0;
}
#endif
