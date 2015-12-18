/**
 * \file main_test_geometry.cpp
 * \brief This is the main file for the test geometry
 *
 * \date Apr 10, 2011
 * \author alexander
 *
 * \ingroup autotest
 */

#include <fstream>
#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "polygons.h"
#include "vector2d.h"

#include "rgb24Buffer.h"
#include "abstractPainter.h"
#include "bmpLoader.h"
#include "convexPolyhedron.h"
#include "mesh3d.h"

using namespace std;
using corecvs::Polygon;
using corecvs::Mesh3D;
using corecvs::AxisAlignedBox3d;

TEST(Geometry, testPolygonInside)
{
    corecvs::Polygon p;
    p.push_back(Vector2dd(0.0,0.0));
    p.push_back(Vector2dd(0.0,1.0));
    p.push_back(Vector2dd(1.0,0.0));

    const unsigned TEST_POINTS = 5;

    Vector2dd tests[TEST_POINTS] = {
            Vector2dd( -0.1,  0.5),
            Vector2dd(  1.0,  1.0),
            Vector2dd( 0.25,  0.25),
            Vector2dd( 0.51,  0.5),
            Vector2dd( 0.49,  0.5)
    };

    bool results[TEST_POINTS] = {false, false, true, false, true};

    for (unsigned i = 0; i < TEST_POINTS; i++)
    {
        cout << tests[i] << " should be " << results[i] << " is " << p.isInside(tests[i]) << endl;
    }
}

//TEST(Geometry, testIntersection)
//{

//    RGB24Buffer image(1000,1000, RGBColor::White());

//    corecvs::Polygon p;
//    for (int i = 0; i < 5; i++)
//    {
//        p.push_back(Vector2dd::FromPolar(-2 * M_PI / 5 * i, 260) + Vector2dd(image.w / 2.0, image.h / 2.0));
//    }


//    AbstractPainter<RGB24Buffer> painter(&image);

//    painter.drawPolygon(p, RGBColor::Black());
//    for (unsigned i = 0; i < p.size(); i++)
//    {
//        Vector2dd p1 = p[i];
//        Vector2dd p2 = p[(i + 1) % p.size()];

//        Vector2dd center = (p1 + p2) / 2.0;
//        Vector2dd decal = (p2 - p1).rightNormal().normalised();
//        decal = center + decal * 8;
//        image.drawLine(center.x(), center.y(), decal.x(), decal.y(), RGBColor::gray(128));
//    }


//    for (int i = 1; i < 10; i++)
//    {
//        Ray2d ray( Vector2dd::FromPolar( M_PI / 20.0 * i + 0.02, 100), Vector2dd(140,180));

//        Vector2dd p1 = ray.getPoint(0.0);
//        Vector2dd p2 = ray.getPoint(8.0);
//        image.drawLine(fround(p1.x()), fround(p1.y()), fround(p2.x()), fround(p2.y()), RGBColor::Yellow());
//        for (int j = 0; j < 8; j++)
//        {
//            Vector2dd p = ray.getPoint(j);
//            image.drawCrosshare1(p.x(), p.y(), RGBColor::Cyan());
//        }

//        double t1, t2;
//        ray.clip<Polygon>(p, t1, t2);

//        p1 = ray.getPoint(t1);
//        p2 = ray.getPoint(t2);

//        cout << "t1 = " << t1 << endl;
//        cout << "t2 = " << t2 << endl;
//        cout << "p1 = " << p1 << endl;
//        cout << "p2 = " << p2 << endl;

//        image.drawLine(fround(p1.x()), fround(p1.y()), fround(p2.x()), fround(p2.y()),
//                       t1 > t2 ? RGBColor::Red() : RGBColor::Green());

//    }


//    BMPLoader().save("out1.bmp", &image);

//}

TEST(Geometry, testIntersection3D)
{
    Mesh3D mesh;
    AxisAlignedBox3d box(Vector3dd(-100, -100, -100), Vector3dd(100, 100, 100));
    ConvexPolyhedron poly(box);

    mesh.switchColor();
    mesh.currentColor = RGBColor::Blue();
    mesh.addAOB(box, false);
    Ray3d ray(Vector3dd(-120, -90, -80), Vector3dd(2.0,1.1,1.5));

    mesh.currentColor = RGBColor::White();
    mesh.addLine(ray.getPoint(0), ray.getPoint(5.0));


    double t1, t2;
    bool result = ray.clip<ConvexPolyhedron> (poly, t1, t2);

    mesh.currentColor = result ? RGBColor::Green() : RGBColor::Red();
    mesh.addLine(ray.getPoint(t1), ray.getPoint(t2));

    ofstream file("test.ply", std::ios::out);
    mesh.dumpPLY(file);
    file.close();

}


TEST(Geometry, rayBasics)
{
    Ray3d ray(Vector3dd::OrtX(), Vector3dd::Zero());
    Vector3dd p(Vector3dd(1.0,1.0,1.0));

    Vector3dd pr = ray.projectOnRay(p);

    cout << "Ray      :" << ray << endl;
    cout << "Point    :" << p  << endl;
    cout << "Projected:" << pr << endl;

    ASSERT_TRUE(pr.notTooFar(Vector3dd::OrtX(), 1e-7));

}

//int main (int /*argC*/, char ** /*argV*/)
//{
//    testIntersection3D();
//    testIntersection();
//    testPolygonInside();
//}
