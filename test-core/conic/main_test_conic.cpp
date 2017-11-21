/**
 * \file main_test_conic.cpp
 * \brief This is the main file for the test conic 
 *
 * \date сент. 29, 2015
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"
#include "core/geometry/conic.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/abstractPainter.h"

#include "core/geometry/mesh3d.h"

using namespace corecvs;

TEST(conic, testCircleHasPoint)
{
    Circle2d c (0, 0, 5);

    ASSERT_TRUE (c.hasPoint(Vector2dd(0,5)));
    ASSERT_TRUE (c.hasPoint(Vector2dd(5,0)));
    ASSERT_TRUE (c.hasPoint(Vector2dd(-3,4)));
    ASSERT_FALSE(c.hasPoint(Vector2dd(-7,4)));
}

TEST(conic, testCircleIntersection)
{

    Circle2d c1(0, 0, 3);
    Circle2d c2(5, 0, 4);

    Vector2dd p1(0.0, 0.0);
    Vector2dd p2(0.0, 0.0);

    ASSERT_TRUE(c1.intersectWith(c2, p1, p2));
    cout << "I1:" << p1 << endl;
    cout << "I2:" << p2 << endl;
    ASSERT_TRUE (c1.hasPoint(p1));
    ASSERT_TRUE (c1.hasPoint(p2));
    ASSERT_TRUE (c2.hasPoint(p1));
    ASSERT_TRUE (c2.hasPoint(p2));

    Circle2d c3(7.5, 0, 4);
    ASSERT_FALSE(c3.intersectWith(c1, p1, p2));
    ASSERT_FALSE(c1.intersectWith(c3, p1, p2));

    ASSERT_TRUE (c3.intersectWith(c2, p1, p2));
    ASSERT_TRUE (c2.intersectWith(c3, p1, p2));


    Circle2d c4(1, 0, 1.49);
    ASSERT_FALSE(c4.intersectWith(c1, p1, p2));
    ASSERT_FALSE(c1.intersectWith(c4, p1, p2));

    cout << "Test <conic> PASSED" << endl;
}

TEST(conic, testCircleIntersection1)
{

    Circle2d c1( 60,  60, 20 * sqrt(10.0));
    Circle2d c2(140, 140, 20 * sqrt(10.0));

    Vector2dd p1(0.0, 0.0);
    Vector2dd p2(0.0, 0.0);

    ASSERT_TRUE(c1.intersectWith(c2, p1, p2));
    cout << "I1:" << p1 << endl;
    cout << "I2:" << p2 << endl;
    ASSERT_TRUE (c1.hasPoint(p1));
    ASSERT_TRUE (c1.hasPoint(p2));
    ASSERT_TRUE (c2.hasPoint(p1));
    ASSERT_TRUE (c2.hasPoint(p2));

    /**/
    RGB24Buffer drawing(220,220);

    //    AbstractPainter<RGB24Buffer>(&drawing).drawCircle(c1, RGBColor::Red  ());
    //    AbstractPainter<RGB24Buffer>(&drawing).drawCircle(c2, RGBColor::Green());
    drawing.drawArc(c1, RGBColor::Red  ());
    drawing.drawArc(c2, RGBColor::Green());
    drawing.setElement(fround(p1.y()), fround(p1.x()), RGBColor::Blue());
    drawing.setElement(fround(p2.y()), fround(p2.x()), RGBColor::Blue());
    BMPLoader().save("circles-int.bmp", &drawing);
    /**/

}

TEST(conic, testSphereIntersection)
{
    Sphere3d s1( 0.0,  0.0,  0.0, 20.0);
    Sphere3d s2(10.0, 20.0, 20.0, 20.0);

    Circle3d c1;
    ASSERT_TRUE(s1.intersectWith(s2, c1));

    for (int i = 0; i < 8; i++) {
        ASSERT_TRUE(s1.hasPoint(c1.getPoint(degToRad(i * 45.0))));
        ASSERT_TRUE(s2.hasPoint(c1.getPoint(degToRad(i * 45.0))));
    }

    Sphere3d s3( 100.0,  100.0,  0.0, 20.0);
    Circle3d c2;
    ASSERT_FALSE(s1.intersectWith(s3, c2));


    Mesh3D mesh;
    mesh.switchColor();
    mesh.setColor(RGBColor::Red());
    mesh.addIcoSphere(s1);

    mesh.setColor(RGBColor::Green());
    mesh.addIcoSphere(s2);

    mesh.setColor(RGBColor::Blue());
    mesh.addCircle(c1);

    mesh.dumpPLY("sphere-int.ply");
}


TEST(conic, testRayIntersection)
{
    Mesh3D mesh;
    Sphere3d s( 0.0,  0.0,  0.0, 20.0);

    mesh.switchColor();
    mesh.setColor(RGBColor::Red());
    mesh.addIcoSphere(s, 2);

    for (double x = -30; x <= 30.0; x+=10.0 )
    {
        for (double y = -30; y <= 30.0; y+=10.0 )
        {
            Ray3d ray;
            ray.a = Vector3dd::OrtZ();
            ray.p = Vector3dd(x,y,-40);

            double d1,d2;
            if (s.intersectWith(ray, d1, d2))
            {
                mesh.setColor(RGBColor::Blue());
                mesh.addLine(ray.getPoint(d1), ray.getPoint(d2));
                CORE_ASSERT_DOUBLE_EQUAL_E((ray.getPoint(d1) - s.c).l2Metric(), 20.0, 1e-7, "Point not on sphere");
                CORE_ASSERT_DOUBLE_EQUAL_E((ray.getPoint(d2) - s.c).l2Metric(), 20.0, 1e-7, "Point not on sphere");
            } else {
                mesh.setColor(RGBColor::Green());
                //mesh.addLine(ray.p, ray.getPoint(50));
            }
        }
    }

    mesh.dumpPLY("sphere-ray.ply");
}

TEST(conic, testRayIntersection1)
{
    Circle2d circle(Vector2dd(10.0, 0.0), 5.0);
    for (double y = -1.0; y <= 1.0; y+=0.1 )
    {
        Ray2d ray(Vector2dd(1.0, y), Vector2dd::Zero());
        //ray.normalise();
        double d1,d2;
        if (circle.intersectWith(ray, d1, d2))
        {

            SYNC_PRINT(("Intersection at %lf %lf at (%lf %lf) and (%lf %lf) dist %lf and %lf\n",
                    d1,
                    d2,
                    ray.getPoint(d1).x(), ray.getPoint(d1).y(),
                    ray.getPoint(d2).x(), ray.getPoint(d2).y(),
                    (ray.getPoint(d1) - circle.c).l2Metric(),
                    (ray.getPoint(d2) - circle.c).l2Metric()
            ));
        } else {
            SYNC_PRINT(("No intersection\n"));
        }

    }
}
