
#include <geometry/convexHull.h>
/**
 * \file main_test_polygon.cpp
 * \brief This is the main file for the test polygon
 *
 * \date дек 27, 2016
 * \author alexander
 *
 * \ingroup autotest
 */

#include <random>
#include <iostream>
#include "gtest/gtest.h"

#include "core/geometry/polygons.h"
#include "core/utils/global.h"
#include "core/math/mathUtils.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/abstractPainter.h"

#include "core/cameracalibration/cameraModel.h"
#include "core/geometry/raytrace/raytraceRenderer.h"
#include "core/geometry/raytrace/raytraceObjects.h"
#include "core/cameracalibration/calibrationDrawHelpers.h"


using namespace std;
using namespace corecvs;

using corecvs::Polygon;


static void drawPolygon(RGB24Buffer *target, const Polygon &p, RGBColor color1, RGBColor color2)
{
    if (p.empty())
        return;

    if (p.size() == 1)
    {
        target->drawLine(p[0].x(), p[0].y(), p[0].x(), p[0].y(), color1);
    }
    for (unsigned i = 0; i < p.size(); i++)
    {
        Vector2dd p1 = p.getPoint    (i);
        Vector2dd p2 = p.getNextPoint(i);
        Vector2dd p =  (p1 + p2) / 2.0;

        target->drawLine(p1, p , color1);
        target->drawLine(p , p2, color2);
    }
}


TEST(polygon, testArea1)
{
    cout << "Starting test <polygon>" << endl;

    Polygon  pentakle = Polygon::RegularPolygon(5, Vector2dd::Zero(), 10);
    Polygon rpentakle = Polygon::Reversed(pentakle);

    cout << "Center :" <<  pentakle.center() << endl;
    cout << "RCenter:" << rpentakle.center() << endl;

    CORE_ASSERT_TRUE_P( pentakle.center().notTooFar(Vector2dd::Zero(), 1e-8), ("Wrong Center"));
    CORE_ASSERT_TRUE_P(rpentakle.center().notTooFar(Vector2dd::Zero(), 1e-8), ("Wrong Center of reversed"));

    double sarea1 =  pentakle.signedArea();
    double sarea2 = rpentakle.signedArea();
    double realArea = 5.0/2.0 * (10.0 * 10.0) * sin(2.0/5.0 * M_PI);

    cout << "True Area:" << realArea << endl;
    cout << "     Area:" << sarea1 << endl;
    cout << "Rev  Area:" << sarea2 << endl;


    CORE_ASSERT_DOUBLE_EQUAL_EP(sarea1, -realArea, 1e-8, ("Wrong Area"));
    CORE_ASSERT_DOUBLE_EQUAL_EP(sarea2, realArea, 1e-8, ("Wrong Area of reversed"));

    cout << "Test <polygon> PASSED" << endl;
}


/**
 * Try to test  non convex polygon
 *
 *
 *    **  **
 *    **  **
 *    **  **
 *    **  **
 *    ******
 *    ******
 **/
TEST(polygon, testArea2)
{
    cout << "Starting test <polygon>" << endl;

    Polygon  p;
    p.push_back(Vector2dd(0.0, 0.0));
    p.push_back(Vector2dd(2.0, 0.0));
    p.push_back(Vector2dd(2.0, 4.0));
    p.push_back(Vector2dd(4.0, 4.0));
    p.push_back(Vector2dd(4.0, 0.0));
    p.push_back(Vector2dd(6.0, 0.0));
    p.push_back(Vector2dd(6.0, 6.0));
    p.push_back(Vector2dd(0.0, 6.0));

    Polygon rp = Polygon::Reversed(p);

    cout << "Center :" <<  p.center() << endl;
    cout << "RCenter:" << rp.center() << endl;

    CORE_ASSERT_TRUE_P( p.center().notTooFar(Vector2dd(3, 2.5), 1e-8), ("Wrong Center"));
    CORE_ASSERT_TRUE_P(rp.center().notTooFar(Vector2dd(3, 2.5), 1e-8), ("Wrong Center of reversed"));

    double sarea1 =  p.signedArea();
    double sarea2 = rp.signedArea();
    double realArea = (6*6) - 2 * 4;

    cout << "True Area:" << realArea << endl;
    cout << "     Area:" << sarea1 << endl;
    cout << "Rev  Area:" << sarea2 << endl;


    CORE_ASSERT_DOUBLE_EQUAL_EP(sarea1, -realArea, 1e-8, ("Wrong Area"));
    CORE_ASSERT_DOUBLE_EQUAL_EP(sarea2, realArea, 1e-8, ("Wrong Area of reversed"));

    cout << "Test <polygon> PASSED" << endl;
}

TEST(polygon, isConvex)
{
Polygon  p =
    {{640, 120}
    , {640, 120}
    , {640, 120}
    , {640, 120}
    , {640, 120}
    , {640, 120}
    , {320, 240}
    , {640, 278.4}
    , {640, 278.4}
    , {640, 283.2}
    , {640, 283.2}
    , {640, 288}
    , {640, 288}
    , {640, 292.8}
    , {640, 292.8}
    , {640, 297.6}
    , {640, 297.6}
    , {640, 302.4}
    , {640, 302.4}
    , {640, 307.2}
    , {640, 307.2}
    , {640, 312}
    , {640, 312}
    , {640, 316.8}
    , {640, 316.8}
    , {640, 321.6}
    , {640, 321.6}
    , {640, 326.4}
    , {640, 326.4}
    , {640, 331.2}
    , {640, 331.2}
    , {640, 336}
    , {640, 336}
    , {640, 340.8}
    , {640, 340.8}
    , {640, 345.6}
    , {640, 345.6}
    , {640, 350.4}
    , {640, 350.4}
    , {640, 355.2}
    , {640, 355.2}
    , {640, 360}
    , {640, 360}
    , {640, 360}};

    ASSERT_TRUE(!p.isConvex());



    RGB24Buffer buffer(1000,1000);
    drawPolygon(&buffer, p, RGBColor::Blue(), RGBColor::Red());
    BMPLoader().save("convex-tesr.bmp", &buffer);
}




TEST(polygon, DISABLED_testGiftWrap1)
{
    Polygon  p;
    std::mt19937 rnd;
    uniform_int_distribution<int> dist(0, 10);

    static const int ITERATIONS = 10;

    for (int count = 0; count < ITERATIONS; count++)
    {
        for (int i = 0; i < 10; i++)
        {
            p.push_back(Vector2dd(dist(rnd), dist(rnd)));
        }

        cout << "Input:" << p << endl;

        Polygon result = ConvexHull::GiftWrap(p);
        cout << "Output:" << result << endl;

        for (size_t i = 0; i < p.size(); i++)
        {
            Vector2dd point = p[i];
            bool hasPoint = result.isInside(point) || result.hasVertex(point);

            if (!hasPoint)
            {
                cout << "Point :" << point << " is not in hull" << flush;
                RGB24Buffer debug(120, 120);

                for (unsigned j = 0; j < result.size(); j++)
                {
                    debug.drawLine(result.getPoint(j) * 10 + Vector2dd(10,10), result.getNextPoint(j) * 10 + Vector2dd(10,10), RGBColor::Yellow());
                }

                for (size_t j = 0; j < p.size(); j++)
                {
                    debug.drawCrosshare3(p[j] * 10 + Vector2dd(10,10), RGBColor::Green());
                }

                debug.drawCrosshare3(p[i] * 10 + Vector2dd(10,10), RGBColor::Red());

                BMPLoader().save("output.bmp", &debug);

            }
            CORE_ASSERT_TRUE(hasPoint, "Convex hull is wrong");
        }
    }
}

TEST(polygon, testRayIntersection)
{

    Ray2d r1 = Ray2d::FromPoints(Vector2dd(5.0,  5.0), Vector2dd(95.0, 95.0));
    Ray2d r2 = Ray2d::FromPoints(Vector2dd(5.0, 95.0), Vector2dd(95.0, 5.0));

    cout << "Ray 1:" << r1 << endl;
    cout << "Ray 2:" << r2 << endl;

    bool hasInt = Ray2d::hasIntersection(r1, r2);
    cout << "Has intersection" << hasInt << endl;
    CORE_ASSERT_TRUE(hasInt, "Intersection is wrong");

    double t1, t2;
    Vector2dd x = Ray2d::intersection(r1, r2, t1, t2);
    cout << "t1:" << t1 << endl;
    cout << "t2:" << t2 << endl;
    cout << "X :" << x << endl;

    CORE_ASSERT_DOUBLE_EQUAL(t1, 0.5, "Wrong intersection");
    CORE_ASSERT_DOUBLE_EQUAL(t2, 0.5, "Wrong intersection");
    CORE_ASSERT_TRUE(x.notTooFar(Vector2dd(50.0, 50.0)), "Intersection corrdinates are wrong");

}

TEST(polygon, testRayNoIntersection)
{
    Ray2d r1 = Ray2d::FromPointAndDirection(Vector2dd( 10.0, 10.0) , Vector2dd(100.0, 0.0));
    Ray2d r2 = Ray2d::FromPointAndDirection(Vector2dd(110.0, 110.0), Vector2dd(150.0, 0.0));

    cout << "Ray 1:" << r1 << endl;
    cout << "Ray 2:" << r2 << endl;

    bool hasInt = Ray2d::hasIntersection(r1, r2);
    cout << "Has intersection:" << hasInt << endl;
    CORE_ASSERT_FALSE(hasInt, "There should be no intersection");

}

/* Some visual check*/
TEST(polygon, testRayIntersectionVisual)
{
    int h = 2000;
    int w = 2000;
    int sample = 40;

    std::mt19937 rng;
    std::uniform_real_distribution<double> runif(2, sample - 2);

    RGB24Buffer *buffer  = new RGB24Buffer(h, w, RGBColor::Black());

    for (int i = 0; i < (h / sample); i++)
        for (int j = 0; j < (w / sample); j++)
        {
            Vector2dd shift(i * sample, j*sample);

            Vector2dd a(runif(rng), runif(rng));
            Vector2dd b(runif(rng), runif(rng));
            Vector2dd a1(runif(rng), runif(rng));
            Vector2dd b1(runif(rng), runif(rng));
            Ray2d r1 = Ray2d::FromPoints(a,a1);
            Ray2d r2 = Ray2d::FromPoints(b,b1);

            double t1, t2;
            Vector2dd x = Ray2d::intersection(r1, r2, t1, t2);

            buffer->drawLine(r1.getStart() + shift, r1.getEnd() + shift, RGBColor::Red());
            buffer->drawLine(r2.getStart() + shift, r2.getEnd() + shift, RGBColor::Blue());

            if (t1 != std::numeric_limits<double>::infinity() && x.isInRect(Vector2dd(0,0), Vector2dd(sample,sample)))
                buffer->drawCrosshare1(x + shift, RGBColor::Green());


        }

    BMPLoader().save("inetrsection.bmp", buffer);
    delete_safe(buffer);

}

TEST(polygon, testWindingNumber)
{
    int h = 200;
    int w = 200;

    Polygon  p;
    p.push_back(Vector2dd(0.0, 0.0));
    p.push_back(Vector2dd(2.0, 0.0));
    p.push_back(Vector2dd(2.0, 4.0));
    p.push_back(Vector2dd(4.0, 4.0));
    p.push_back(Vector2dd(4.0, 0.0));
    p.push_back(Vector2dd(6.0, 0.0));
    p.push_back(Vector2dd(6.0, 1.0));
    p.push_back(Vector2dd(1.0, 2.0));
    p.push_back(Vector2dd(6.0, 3.0));
    p.push_back(Vector2dd(6.0, 6.0));
    p.push_back(Vector2dd(0.0, 6.0));

    p.transform(Matrix33::ShiftProj(10,10) * Matrix33::Scale2(30));

    RGB24Buffer *buffer  = new RGB24Buffer(h, w, RGBColor::Black());
    AbstractPainter<RGB24Buffer> painter(buffer);

    RGBColor palette[] =
    {
        RGBColor(0x762a83u),
        RGBColor(0xaf8dc3u),
        RGBColor(0xe7d4e8u),
        RGBColor(0xd9f0d3u),
        RGBColor(0x7fbf7bu),
        RGBColor(0x1b7837u)
    };


    for (int i = 0; i < h ; i++)
    {
        for (int j = 0; j < w ; j++)
        {
            int wn = p.windingNumber(Vector2dd(j,i));
            int color = wn + 2;
            if (color < 0) color = 0;
            if (color >= (int)CORE_COUNT_OF(palette)) color = CORE_COUNT_OF(palette) - 1;
            buffer->element(i, j)  = palette[color];
        }
    }

    painter.drawPolygon(p, RGBColor::Blue());

    BMPLoader().save("winding.bmp", buffer);
    delete_safe(buffer);

}


TEST(polygon, testIntersection)
{
    int h = 400;
    int w = 400;

    Polygon  p1 = Polygon::RegularPolygon(5, Vector2dd(w / 2 , h / 2), 180);
    Polygon  p2 = Polygon::Reversed(Polygon::RegularPolygon(5, Vector2dd(w / 2 , h / 2), 180, degToRad(36)));

    p1[0] = Vector2dd(w / 2, h / 2);

    RGB24Buffer *buffer  = new RGB24Buffer(h, w, RGBColor::White());
    AbstractPainter<RGB24Buffer> painter(buffer);

    PolygonCombiner combiner;
    combiner.pol[0] = p1;
    combiner.pol[1] = p2;

    combiner.prepare();
    Polygon p3 = combiner.intersection();

    combiner.drawDebug(buffer, RGBColor::Navy(), RGBColor::Brown(), RGBColor::Black());
    //painter.drawPolygon(p3, RGBColor::Magenta());

    BMPLoader().save("intersect.bmp", buffer);

    painter.drawPolygon(p3, RGBColor::Magenta());
    BMPLoader().save("intersect1.bmp", buffer);
    delete_safe(buffer);
}

TEST(polygon, testIsSelfintersection)
{
    int h = 200;
    int w = 200;

    Polygon p1 = Polygon::RegularPolygon(6, Vector2dd(w / 2 , h / 2), 80);
    Polygon p2 = p1;
    std::swap(p2[0], p2[3]);

    RGB24Buffer buffer(h, w, RGBColor::Black());
    drawPolygon(&buffer, p2, RGBColor::Red(), RGBColor::Blue());
    BMPLoader().save("selfintersect.bmp", &buffer);

    CORE_ASSERT_TRUE_P( p2.hasSelfIntersection(), ("Should be selfintersecting"     ));
    CORE_ASSERT_TRUE_P(!p1.hasSelfIntersection(), ("Should not be selfintersecting" ));

}

TEST(polygon, testConvexConversion)
{
    int h = 400;
    int w = 400;

    Polygon p1 = Polygon::RegularPolygon(6, Vector2dd(w / 2 , h / 2), 180);
    ConvexPolygon cp = p1.toConvexPolygon();
    Polygon p2 = Polygon::FromConvexPolygon(cp);

    cout << "Initial" << p1 << endl;
    cout << "Result " << p2 << endl;

    RGB24Buffer *buffer  = new RGB24Buffer(h, w, RGBColor::Black());
    AbstractPainter<RGB24Buffer> painter(buffer);

    if (0)
    {
        for (int i = 0; i < buffer->h; i++)
        {
            for (int j = 0; j < buffer->w; j++)
            {
                buffer->element(i,j) = cp.isInside(Vector2dd(j,i)) ? RGBColor::White() : RGBColor::Black();
            }
        }
    }

    painter.drawPolygon(p1, RGBColor::Magenta());
    painter.drawPolygon(p2, RGBColor::Green());

    BMPLoader().save("convex.bmp", buffer);


}


int TEST_FILED_H = 400;
int TEST_FILED_W = 400;

Polygon testTwoPolygons(Polygon &p1, Polygon &p2, const std::string &name)
{

    RGB24Buffer *buffer  = new RGB24Buffer(TEST_FILED_H, TEST_FILED_W, RGBColor::Black());
    AbstractPainter<RGB24Buffer> painter(buffer);

    PolygonCombiner combiner;
    combiner.pol[0] = p1;
    combiner.pol[1] = p2;

    combiner.prepare();
    cout << "Structure after preparation" << endl;
    cout << combiner << endl;
    combiner.validateState();

    Polygon pIntr = combiner.intersection();
    Polygon pComb = combiner.combination();
    cout << "Structure after intersection" << endl;
    cout << combiner << endl;


    painter.drawPolygon(p1, RGBColor::Yellow());
    painter.drawPolygon(p2, RGBColor::Cyan());


    for (int i = 0; i < TEST_FILED_H; i++)
        for (int j = 0; j < TEST_FILED_W; j++)
        {
            if (pIntr.isInside(Vector2dd(j,i)))
            {
                buffer->element(i,j) = RGBColor::Gray();
            }
        }


    //painter.drawPolygon(p3, RGBColor::Magenta());
    /*for (size_t i = 0; i < p3.size(); i++)
    {
        Vector2dd point = p3.getPoint(i);
        buffer->drawLine(point, p3.getNextPoint(i), RGBColor::rainbow((double)i / (p3.size())));
        painter.drawFormat(point.x(), point.y(), RGBColor::White(), 1, "%d", i);
    }*/


    combiner.drawDebug(buffer);
    cout << "Result intersection: " << pIntr << endl;
    cout << "Result combination : " << pComb << endl;

    cout << combiner << endl;

    painter.drawPolygon(pComb, RGBColor::Magenta());

    BMPLoader().save(name, buffer);
    delete_safe(buffer);

    return pIntr;
}

void testTwoPolygonsIntersection(Polygon &p1, Polygon &p2, const std::string &name, bool debug = true)
{

    RGB24Buffer *buffer  = new RGB24Buffer(TEST_FILED_H, TEST_FILED_W, RGBColor::Black());
    AbstractPainter<RGB24Buffer> painter(buffer);

    PolygonCombiner combiner;
    combiner.pol[0] = p1;
    combiner.pol[1] = p2;

    combiner.prepare();
    cout << "Structure after preparation" << endl;
    cout << combiner << endl;
    combiner.validateState();

    vector<Polygon> p3 = combiner.intersectionAll();
    cout << "intersectionAll returned " << p3.size() << endl;
    cout << "Structure after intersection" << endl;
    cout << combiner << endl;


    painter.drawPolygon(p1, RGBColor::Yellow());
    painter.drawPolygon(p2, RGBColor::Cyan());


    for (int i = 0; i < TEST_FILED_H; i++)
        for (int j = 0; j < TEST_FILED_W; j++)
        {
            for(size_t v = 0; v < p3.size(); v++)
            {
                if (p3[v].isInside(Vector2dd(j,i)))
                {
                    buffer->element(i,j) = RGBColor::Gray();
                }
            }
        }

    if (debug) {
        combiner.drawDebug(buffer);
    }
    cout << combiner << endl;

    BMPLoader().save(name, buffer);
    delete_safe(buffer);
}

TEST(polygon, eightPoints)
{
    Polygon  p2 = Polygon::RegularPolygon(8, Vector2dd(200, 80), 80);
    Polygon  p1 = Polygon::RegularPolygon(8, Vector2dd(200,120), 80);
    Polygon  p3 = testTwoPolygons(p1, p2, "poly-8.bmp");

    CORE_ASSERT_TRUE_P(p3.size() == 8, ("We have polygon of size %d\n", (int)p3.size()));
}

TEST(polygon, triangles)
{
    Polygon  p2 = Polygon::RegularPolygon(3, Vector2dd(200, 80), 80);
    Polygon  p1 = Polygon::RegularPolygon(3, Vector2dd(210,120), 80);
    Polygon  p3 = testTwoPolygons(p1, p2, "poly-3.bmp");

    CORE_ASSERT_TRUE_P(p3.size() == 3, ("We have polygon of size %d\n", (int)p3.size()));
}

TEST(polygon, simpleNonConvex)
{
    Polygon  p2;
    Polygon  p1;

    p1.push_back(Vector2dd( 20, 200));
    p1.push_back(Vector2dd(300,  20));
    p1.push_back(Vector2dd(190, 200));
    p1.push_back(Vector2dd(300, 380));

    p2.push_back(Vector2dd(380, 200));
    p2.push_back(Vector2dd(100,  20));
    p2.push_back(Vector2dd(210, 200));
    p2.push_back(Vector2dd(100, 380));

    testTwoPolygons(p1, p2, "poly-nonconvex-1.bmp");
}

TEST(polygon, twoSeparateIntersections)
{
    Polygon  p2 = Polygon::Reversed(Polygon::RegularPolygon(5, Vector2dd(TEST_FILED_W / 2 , TEST_FILED_H / 2), 180, degToRad(72)));
    p2[0] = Vector2dd(TEST_FILED_W / 2, TEST_FILED_H / 2);
    Polygon  p1 = p2;
    p2.transform(Matrix33::ShiftProj(TEST_FILED_W, 10) * Matrix33::MirrorYZ());
    testTwoPolygons(p1, p2, "poly-nonconvex-2.bmp");
}


TEST(polygon, twoSeparateIntersections1)
{
    Polygon  p2 = Polygon::Reversed(Polygon::RegularPolygon(5, Vector2dd(TEST_FILED_W / 2 , TEST_FILED_H / 2), 180, degToRad(72)));
    p2[0] = Vector2dd(TEST_FILED_W / 2, TEST_FILED_H / 2);
    Polygon  p1 = p2;
    p2.transform(Matrix33::ShiftProj(TEST_FILED_W + 15, 10) * Matrix33::MirrorYZ());
    testTwoPolygons(p1, p2, "poly-nonconvex-3.bmp");
}

TEST(polygon, trivialIntersection)
{
    Polygon  p2 = Polygon::Reversed(Polygon::RegularPolygon(5, Vector2dd(TEST_FILED_W / 2 , TEST_FILED_H / 2), 180, degToRad(72)));
    Polygon  p1 = p2.transformed(Matrix33::RotationZ(degToRad(36)));
    p2.transform(Matrix33::ShiftProj(TEST_FILED_W, 10) * Matrix33::MirrorYZ());
    testTwoPolygons(p1, p2, "poly-trivial.bmp");
}

TEST(polygon, collinearIntersection)
{
    Polygon  p2 = Polygon::FromRectagle(Rectangled::FromCorners(Vector2dd(60, 60), Vector2dd(TEST_FILED_W - 60,TEST_FILED_H - 60)));
    Polygon  p1 = Polygon::FromRectagle(Rectangled::FromCorners(Vector2dd(60,100), Vector2dd(TEST_FILED_W - 100,TEST_FILED_H - 60)));
    testTwoPolygons(p1, p2, "poly-collinear.bmp");
}

TEST(polygon, noIntersection)
{
    Polygon  p2 = Polygon::FromRectagle(Rectangled(Vector2dd(10,10), Vector2dd( 40, 40)));
    Polygon  p1 = Polygon::FromRectagle(Rectangled(Vector2dd(60,60), Vector2dd(290,290)));
    Polygon  p3 = testTwoPolygons(p1, p2, "poly-no-intersection.bmp");
    CORE_ASSERT_TRUE_P(p3.size() == 0, ("There should be no intersection. But we have polygon of size %d\n", (int)p3.size()));
}

TEST(polygon, DISABLED_intersectionTouch)
{
    Polygon  p2 = Polygon::FromRectagle(Rectangled(Vector2dd( 10, 10), Vector2dd( 100, 100)));
    Polygon  p1 = Polygon::FromRectagle(Rectangled(Vector2dd(110,110), Vector2dd(290,290)));
    Polygon  p3 = testTwoPolygons(p1, p2, "poly-touch.bmp");
    CORE_ASSERT_TRUE_P(p3.size() == 0, ("There should be no intersection. But we have polygon of size %d\n", (int)p3.size()));
}

TEST(polygon, insideIntersection)
{
    Polygon  p2 = Polygon::FromRectagle(Rectangled(Vector2dd(10,10), Vector2dd(250,250)));
    Polygon  p1 = Polygon::FromRectagle(Rectangled(Vector2dd(60,60), Vector2dd(140, 140)));
    Polygon  p3 = testTwoPolygons(p1, p2, "poly-inside.bmp");

    CORE_ASSERT_TRUE(p3[0].notTooFar(Vector2dd (60,  60)), "Fail with one poligon inside the other");
    CORE_ASSERT_TRUE(p3[1].notTooFar(Vector2dd(200,  60)), "Fail with one poligon inside the other");
    CORE_ASSERT_TRUE(p3[2].notTooFar(Vector2dd(200, 200)), "Fail with one poligon inside the other");
    CORE_ASSERT_TRUE(p3[3].notTooFar(Vector2dd( 60, 200)), "Fail with one poligon inside the other");
}


TEST(polygon, tortureTest1)
{
    Polygon  p2;
    Polygon  p1;

    double step = 80;

    for (int i = 1; i < 400 / step; i++)
    {
        double y =  i * step;
        double x = (i % 2) ? 10 : 350;
        p1.push_back(Vector2dd(x, y));
    }

    p1.push_back(Vector2dd(385, 380));
    p1.push_back(Vector2dd(385, 10));

    p2.push_back(Vector2dd(380, 200));
    p2.push_back(Vector2dd(100,  20));
    p2.push_back(Vector2dd(210, 200));
    p2.push_back(Vector2dd(100, 380));

    testTwoPolygons            (p1, p2, "poly-torture.bmp");
    testTwoPolygonsIntersection(p1, p2, "poly-torture-all.bmp");
}

TEST(polygon, tortureTest2)
{
    Polygon  p2;
    Polygon  p1;

    double step = 40;

    for (int i = 1; i < 400 / step; i++)
    {
        double y =  i * step;
        double x = (i % 2) ? 5 : 350;
        p1.push_back(Vector2dd(x, y));
    }

    p1.push_back(Vector2dd(385, 380));
    p1.push_back(Vector2dd(385, 10));

    p2 = p1;
    p2.transform(Matrix33::SwapXY());

    testTwoPolygons            (p1, p2, "poly-torture2.bmp");
    testTwoPolygonsIntersection(p1, p2, "poly-torture-all2.bmp", false);
}

TEST(polygon, tortureTest3)
{
    Polygon  p1 = { {-2.90149, 0.0805642}
                    , {-3.0181, 0.0605381}
                    , {-3.13391, 0.0444257}
                    , {3.04539, 0.0343246}
                    , {2.96102, 0.0307865}
                    , {2.89836, 0.0328962}
                    , {2.87241, 0.0980895}
                    , {2.84955, 0.170706}
                    , {2.83127, 0.248412}
                    , {2.81919, 0.327795}
                    , {2.81451, 0.404978}
                    , {2.81469, 0.405896}
                    , {2.8774, 0.446287}
                    , {2.97198, 0.48808}
                    , {3.09762, 0.523923}
                    , {-3.0405, 0.545819}
                    , {-2.89553, 0.550269}
                    , {-2.89553, 0.549402}
                    , {-2.89588, 0.459391}
                    , {-2.89676, 0.361218}
                    , {-2.89809, 0.261286}
                    , {-2.89971, 0.166112}};

    Polygon  p2 = {{-2.64873, 0.0640694}
                   , {-2.75313, 0.0490903}
                   , {-2.87685, 0.0368737}
                   , {-3.00952, 0.0297115}
                   , {-3.13618, 0.0289084}
                   , {3.03898, 0.0340167}
                   , {3.01716, 0.112584}
                   , {2.99757, 0.201737}
                   , {2.98153, 0.297595}
                   , {2.97075, 0.394283}
                   , {2.96654, 0.485397}
                   , {2.96695, 0.48644}
                   , {3.09391, 0.523037}
                   , {-3.02993, 0.546339}
                   , {-2.86019, 0.548172}
                   , {-2.70778, 0.529923}
                   , {-2.58856, 0.500558}
                   , {-2.58856, 0.499719}
                   , {-2.59199, 0.413882}
                   , {-2.60081, 0.322209}
                   , {-2.61406, 0.230079}
                   , {-2.63048, 0.142738}};

    RGB24Buffer buffer(1500, 1500);
    BMPLoader tracer;

    p1 = ConvexHull::GiftWrap(p1);
    p2 = ConvexHull::GiftWrap(p2);


    SYNC_PRINT(("Prefail trace"));
    cout << "First : " << p1 << endl;
    cout << "Second: " << p2 << endl;
    PolygonCombiner combiner(p1, p2);
    combiner.prepare();

    Rectangled r = combiner.getDebugRectangle();
    Matrix33 transform = Matrix33::Scale2(buffer.w / r.size.x(), buffer.h / r.size.y()) * Matrix33::ShiftProj(-r.corner.x(), -r.corner.y());;



    Polygon pol[] = {p1, p2};




    for (int p = 0; p < 2; p++)
    {
        for (int i = 0; i < (int)pol[p].size(); i++)
        {
            Vector2dd v1 = transform * pol[p].getPoint(i);
            Vector2dd v2 = transform * pol[p].getNextPoint(i);
            buffer.drawLine(v1, v2, p == 0 ? RGBColor::Yellow() : RGBColor::Cyan());
        }
    }


    RGB24Buffer debug(1500, 1500);
    combiner.drawDebugAutoscale(&debug);
    tracer.save("debug_intersection.bmp", &debug);

    Polygon p3 = combiner.intersection();


    for (int i = 0; i < (int)p3.size(); i++)
    {
        Vector2dd v1 = transform * p3.getPoint(i);
        Vector2dd v2 = transform * p3.getNextPoint(i);
        buffer.drawLine(v1, v2, RGBColor::Green());
    }

    tracer.save("debug_input.bmp", &buffer);
}


TEST(polygon, CameraView)
{
    CameraModel cam1, cam2;

    cam1.intrinsics.reset(new PinholeCameraIntrinsics(Vector2dd(400,400), degToRad(50)));
    cam2.intrinsics.reset(new PinholeCameraIntrinsics(Vector2dd(400,400), degToRad(50)));

    cam1.setLocation(Affine3DQ::Shift(-10, 0, 0) * Affine3DQ::RotationY(degToRad(10)) * Affine3DQ::RotationX(degToRad(4)));
    cam1.setLocation(Affine3DQ::Shift( 10, 0, 0) * Affine3DQ::RotationY(degToRad(-10)));


    Sphere3d s(Vector3dd::Zero(), 200);
    RaytraceableSphere sphere(s);
    Matrix44 sphereRot    = Matrix44::RotationX(degToRad(90));
    Matrix44 sphereRotInv = Matrix44::RotationX(degToRad(-90));

    Polygon in;
    in.push_back(Vector2dd(-200,-200));
    in.push_back(Vector2dd(-100,-200));
    in.push_back(Vector2dd(-  0,-200));
    in.push_back(Vector2dd( 100,-200));
    in.push_back(Vector2dd( 200,-200));

    in.push_back(Vector2dd( 200,-100));
    in.push_back(Vector2dd( 200,-  0));
    in.push_back(Vector2dd( 200, 100));
    in.push_back(Vector2dd( 200, 200));

    in.push_back(Vector2dd( 100, 200));
    in.push_back(Vector2dd(   0, 200));
    in.push_back(Vector2dd(-100, 200));
    in.push_back(Vector2dd(-200, 200));

    in.push_back(Vector2dd(-200, 100));
    in.push_back(Vector2dd(-200,   0));
    in.push_back(Vector2dd(-200,-100));
    in.push_back(Vector2dd(-200,-200));

    for (size_t i = 0; i < in.size(); i++)
    {
        in[i] += Vector2dd(200, 200);
    }

    Mesh3D mesh;
    mesh.switchColor(true);
    CalibrationDrawHelpers drawer;

    mesh.setColor(RGBColor::Red());
    drawer.drawCamera(mesh, cam1, 3.0);

    mesh.setColor(RGBColor::Green());
    drawer.drawCamera(mesh, cam2, 3.0);

    mesh.setColor(RGBColor::Blue());
    mesh.addSphere(s, 2.0);

    std::vector<Vector3dd> proj1;
    std::vector<Vector3dd> proj2;

    Polygon p1, p2;

    for (size_t i = 0; i < in.size(); i++)
    {
        Ray3d ray1 = cam1.rayFromPixel(in[i]).normalised();
        RayIntersection r1;
        r1.ray = ray1;
        sphere.intersect(r1);
        proj1.push_back(r1.getPoint());
        mesh.addSphere(r1.getPoint(), 1);
        mesh.addLine(ray1.getPoint(0.0), ray1.getPoint(20.0));

        Ray3d ray2 = cam2.rayFromPixel(in[i]).normalised();
        RayIntersection r2;
        r2.ray = ray2;
        sphere.intersect(r2);
        proj2.push_back(r2.getPoint());
        mesh.addSphere(r2.getPoint(), 1);
        mesh.addLine(ray2.getPoint(0.0), ray2.getPoint(20.0));


        p1.push_back(Vector3dd::toSpherical(sphereRot * r1.getPoint()).xy());
        p2.push_back(Vector3dd::toSpherical(sphereRot * r2.getPoint()).xy());
    }


    PolygonCombiner combiner(p1, p2);
    combiner.prepare();
    Polygon p3 = combiner.intersection();

    {
        int h = 4000;
        int w = 4000;
        RGB24Buffer *buffer  = new RGB24Buffer(h, w, RGBColor::Black());
        AbstractPainter<RGB24Buffer> painter(buffer);
        painter.drawPolygon(p3.transformed(Matrix33::ShiftProj(2000, 2000) * Matrix33::Scale2(700)), RGBColor::Yellow());

        // This is for debug only
        Polygon p1large = p1.transformed(Matrix33::ShiftProj(2000, 2000) * Matrix33::Scale2(700));
        Polygon p2large = p2.transformed(Matrix33::ShiftProj(2000, 2000) * Matrix33::Scale2(700));
        PolygonCombiner largeCombiner(p1large, p2large);
        largeCombiner.prepare();
        largeCombiner.drawDebug(buffer);
        BMPLoader().save("spherical.bmp", buffer);
        delete_safe(buffer);
    }

    mesh.setColor(RGBColor::Green());
    for (size_t i = 0; i < in.size(); i++)
    {
        mesh.addLine(proj1[i], proj1[(i + 1) % in.size()]);
        mesh.addLine(proj2[i], proj2[(i + 1) % in.size()]);
    }

    mesh.setColor(RGBColor::Yellow());
    for (size_t i = 0; i < p3.size(); i++)
    {
        Vector2dd curr = p3.getPoint((int)i);
        Vector2dd next = p3.getNextPoint((int)i);

        Vector3dd sph1 = sphereRot.inverted() * Vector3dd::FromSpherical(curr.x(), curr.y(), s.r);
        Vector3dd sph2 = sphereRot.inverted() * Vector3dd::FromSpherical(next.x(), next.y(), s.r);

        mesh.addLine(sph1, sph2);
    }

    mesh.dumpPLY("out.ply");

}


TEST(Rectangle, noIntersection)
{
    Rectangled r1(100,100, 10,10);
    Rectangled r2(200,200, 10,10);

    Rectangled r3 = r1.intersect(r2);
    CORE_ASSERT_TRUE(r3.isEmpty(), "Wrong intrsection");
}

TEST(Rectangle, intersection)
{
    Rectangled r1(100,100, 110,110);
    Rectangled r2(200,200, 10,10);

    Rectangled r3 = r1.intersect(r2);
    cout << r3 << endl;

    Rectangled r4 = r2.intersect(r1);
    cout << r4 << endl;
    CORE_ASSERT_TRUE(r3.ulCorner() == Vector2dd(200,200), "Wrong intrsection");
    CORE_ASSERT_TRUE(r3.lrCorner() == Vector2dd(210,210), "Wrong intrsection");

    CORE_ASSERT_TRUE(r4.ulCorner() == Vector2dd(200,200), "Wrong intrsection");
    CORE_ASSERT_TRUE(r4.lrCorner() == Vector2dd(210,210), "Wrong intrsection");

}
