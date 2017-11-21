/**
 * \file main_test_quadric.cpp
 * \brief This is the main file for the test quadric 
 *
 * \date мар 25, 2017
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <random>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/geometry/ellipse.h"
#include "core/geometry/ellipseFit.h"

using namespace std;
/* Ok. In your portable code you should respect Microsoft craze, just because they say it's a dominant platform. */
//using namespace corecvs;
/* You could not use your own namespace in your own test, beacuse Microsoft used "Ellipse" a bit earlier. */
using corecvs::BMPLoader;
using corecvs::RGB24Buffer;
using corecvs::SecondOrderCurve;
using corecvs::RGBColor;
using corecvs::HLineSpanInt;
using corecvs::Vector2dd;
using corecvs::Matrix33;
using corecvs::Vector2d32;
using corecvs::fround;
using corecvs::degToRad;
using corecvs::EllipseFit;

void drawDirect(RGB24Buffer *buffer, const SecondOrderCurve &curve, RGBColor color)
{
    for (int i = 0; i < buffer->h; i++)
        for (int j = 0; j < buffer->w; j++)
        {
            if (curve.isInside(j,i))
            {
                buffer->element(i,j) = color;
            }
        }
}


void drawDirect(RGB24Buffer *buffer, const corecvs::Ellipse &curve, RGBColor color)
{
    int MAX = 850;
    for (int k = 0; k < MAX; k++)
    {
        double v = 2 * M_PI / MAX * k;
        Vector2dd point = curve.pointFromParam(v);
        Vector2d32 p(fround(point.x()), fround(point.y()));
        if (buffer->isValidCoord(p)) {
            buffer->element(p) = color;
        }
    }
}

TEST(quadric, testquadric)
{
    cout << "Starting test <quadric>" << endl;
    unique_ptr<RGB24Buffer> buffer(new RGB24Buffer(200,200, RGBColor::Black()));

    SecondOrderCurve circle = SecondOrderCurve::FromCircle(Vector2dd(10,10), 10);
    circle.prettyPrint();

    drawDirect(buffer.get(), circle, RGBColor::Red());

    SecondOrderCurve ellipse = circle.transformed(Matrix33::ShiftProj(80,30) * Matrix33::Scale3(1,3));
    drawDirect(buffer.get(), ellipse, RGBColor::Green());


    BMPLoader().save("quadric1.bmp", buffer.get());
    cout << "Test <quadric> PASSED" << endl;

}


TEST(quadric, testConversion)
{
    cout << "Starting test <quadric>" << endl;
    unique_ptr<RGB24Buffer> buffer(new RGB24Buffer(200,200, RGBColor::Black()));

    SecondOrderCurve circle = SecondOrderCurve::FromCircle(Vector2dd(10,10), 20);
    circle.prettyPrint();
    SecondOrderCurve ellipse = circle.transformed(Matrix33::ShiftProj(80,30) * Matrix33::RotationZ(degToRad(60)) * Matrix33::Scale3(1,3));
    drawDirect(buffer.get(), ellipse, RGBColor::Green());

    corecvs::Ellipse ellipse1 = corecvs::Ellipse::FromQuadric(ellipse);
    drawDirect(buffer.get(), ellipse1, RGBColor::Blue());

    SecondOrderCurve ellipse2 = ellipse1.toSecondOrderCurve().transformed(Matrix33::ShiftProj(40, 0));
    drawDirect(buffer.get(), ellipse2, RGBColor::Red());

    BMPLoader().save("quadricconv.bmp", buffer.get());
    cout << "Test <quadric> PASSED" << endl;
}

TEST(quadric, testSolving)
{
    cout << "Starting test <quadric>" << endl;
    unique_ptr<RGB24Buffer> buffer(new RGB24Buffer(200,200, RGBColor::Black()));

    SecondOrderCurve circle = SecondOrderCurve::FromCircle(Vector2dd(10,10), 20);
    circle.prettyPrint();
    SecondOrderCurve ellipse = circle.transformed(Matrix33::ShiftProj(80,30) * Matrix33::RotationZ(degToRad(60)) * Matrix33::Scale3(1,3));

    for (int i = 0; i < buffer->h; i++)
    {
        double x1, x2;
        if (!ellipse.solveForX(i, x1, x2))
            continue;
        for (int j= x1; j <= x2; j++)
        {
            if (buffer->isValidCoord(i, j)) {
                buffer->element(i, j) = RGBColor::Yellow();
            }
        }
    }


    ellipse.transform(Matrix33::ShiftProj(40,40));
    for (int j = 0; j < buffer->w; j++)
    {
        double y1, y2;
        if (!ellipse.solveForY(j, y1, y2))
            continue;
        for (int i = y1; i <= y2; i++)
        {
            if (buffer->isValidCoord(i, j)) {
                buffer->element(i, j) = RGBColor::Cyan();
            }
        }
    }


    BMPLoader().save("quadricsolve.bmp", buffer.get());
    cout << "Test <quadric> PASSED" << endl;
}


TEST(quadric, fitellipse)
{
    unique_ptr<RGB24Buffer> buffer(new RGB24Buffer(200, 200));

    std::mt19937 rng;
    std::uniform_real_distribution<double> runif(-7, 7);

    EllipseFit fitter;

    for (int i = 0; i < 20; i++) {
        Vector2dd point = Vector2dd::FromPolar(i / 10.0, 40.0) * Vector2dd(1.0, 2.0);
        point = Matrix33::RotationZ(degToRad(30)) * point;
        point += Vector2dd(100 + runif(rng),100 + runif(rng));

        fitter.addPoint(point);
    }


    SecondOrderCurve ellipse = fitter.solveDummy();
    drawDirect(buffer.get(), ellipse, RGBColor::Green());

    for (size_t i = 0; i < fitter.points.size(); i++)
    {
        Vector2dd point = fitter.points[i];
        buffer->drawCrosshare3(point,RGBColor::Red());
    }

    BMPLoader().save("fitellipse.bmp", buffer.get());
}

#ifdef WITH_BLAS
TEST(quadric, fitellipseblas)
{
    unique_ptr<RGB24Buffer> buffer(new RGB24Buffer(200, 200));

    std::mt19937 rng;
    std::uniform_real_distribution<double> runif(-7, 7);

    EllipseFit fitter;

    for (int i = 0; i < 20; i++) {
        Vector2dd point = Vector2dd::FromPolar(i / 10.0, 40.0) * Vector2dd(1.0, 2.0);
        point = Matrix33::RotationZ(degToRad(30)) * point;
        point += Vector2dd(100 + runif(rng),100 + runif(rng));

        fitter.addPoint(point);
    }


    SecondOrderCurve ellipse = fitter.solveBLAS();
    drawDirect(buffer.get(), ellipse, RGBColor::Green());

    for (size_t i = 0; i < fitter.points.size(); i++)
    {
        Vector2dd point = fitter.points[i];
        buffer->drawCrosshare3(point,RGBColor::Red());
    }

    BMPLoader().save("fitellipseblas.bmp", buffer.get());
}
#endif


TEST(quadric, fitquadric)
{
    unique_ptr<RGB24Buffer> buffer(new RGB24Buffer(200, 200));

    std::mt19937 rng;
    std::uniform_real_distribution<double> runif(-1, 1);

    EllipseFit fitter;

    for (int i = 0; i < 20; i++) {
        Vector2dd point = Vector2dd::FromPolar(i / 10.0, 40.0) * Vector2dd(1.0, 2.0);
        point = Matrix33::RotationZ(degToRad(30)) * point;
        point += Vector2dd(100 + runif(rng),100 + runif(rng));

        fitter.addPoint(point);
    }


    SecondOrderCurve quadric = fitter.solveQuadricDummy();
    drawDirect(buffer.get(), quadric, RGBColor::Green());

    for (size_t i = 0; i < fitter.points.size(); i++)
    {
        Vector2dd point = fitter.points[i];
        buffer->drawCrosshare3(point,RGBColor::Red());
    }

    BMPLoader().save("fitquadric.bmp", buffer.get());
}

TEST(quadric, drawEllipse)
{
    corecvs::Ellipse ellipse;

    ellipse.axis   = Vector2dd(90, 20);
    ellipse.angle  = degToRad(75);
    ellipse.center = Vector2dd(100, 100);

    unique_ptr<RGB24Buffer> buffer(new RGB24Buffer(200, 200));

    corecvs::EllipseSpanIterator outer(ellipse);
    while (outer.hasValue())
    {
        HLineSpanInt span = outer.getSpan();
        for (int s1 = span.x1; s1 < span.x2; s1++ )
        {
            if (buffer->isValidCoord(span.y(), s1))
            {
                buffer->element(span.y(), s1) = RGBColor::Amber();
            }
        }
        outer.step();
    }

    BMPLoader().save("ellipsedraw.bmp", buffer.get());

}



