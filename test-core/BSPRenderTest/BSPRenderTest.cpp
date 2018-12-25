/**
 * \file main_test_draw.cpp
 * \brief This is the main file for the test draw
 *
 * \date Apr 19, 2011
 * \author alexander
 *
 * \ingroup autotest
 */

#include <iostream>
#include <random>
#include <fstream>

#include "core/geometry/renderer/attributedTriangleSpanIterator.h"

#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/geometry/mesh3d.h"
#include "core/cameracalibration/cameraModel.h"

#include "core/geometry/polygonPointIterator.h"
#include "core/buffers/bufferFactory.h"

using namespace corecvs;

TEST(BSPRender, levelDraw)
{
    int h = 720;
    int w = 720;

    corecvs::Polygon p;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    p.push_back(Vector2dd(0, 0));

    p.push_back(Vector2dd(299, 59));
    p.push_back(Vector2dd(239, 239));
    p.push_back(Vector2dd(479, 299));
    p.push_back(Vector2dd(479, 59));

    p.push_back(Vector2dd(719, 0));
    p.push_back(Vector2dd(719, 479));
    p.push_back(Vector2dd(539, 719));

    p.push_back(Vector2dd(479, 719));
    p.push_back(Vector2dd(479, 479));
    p.push_back(Vector2dd(299, 419));
    p.push_back(Vector2dd(299, 599));

    p.push_back(Vector2dd(0, 719));

    AbstractPainter<RGB24Buffer> painter(buffer);
    painter.drawPolygon(p, RGBColor::White());
    BMPLoader().save("levelPolygon.bmp", buffer);



//    PolygonPointIterator it(p);
//    for (Vector2d<int> point : it) {
//        //cout << point << endl;
//        ASSERT_TRUE(buffer->isValidCoord(point));
//        buffer->element(point) += RGBColor::Red();
//    }

//    BMPLoader().save("polygon2.bmp", buffer);
    delete_safe(buffer);
}


TEST(Draw, testPoly)
{
    int h = 1024;
    int w = 720;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
    Vector2dd center(w / 2.0, h / 2.0);

    AbstractPainter<RGB24Buffer> painter(buffer);

    corecvs::Polygon p;
    for (int i = 0; i < 7; i++) {
        p.push_back(Vector2dd::FromPolar((2 * M_PI / 7.0) * i, 100.0) + center);
    }

    cout << p << std::endl;

    painter.drawPolygon(p, RGBColor::Blue());

    PolygonPointIterator it(p);
    for (Vector2d<int> p: it) {
        if (buffer->isValidCoord(p)) {
            buffer->element(p) += RGBColor(140, 140, 0);
        }
    }

    BMPLoader().save("polygon.bmp", buffer);
    delete_safe(buffer);
}


TEST(Draw, testPoly1)
{
    int h = 240;
    int w = 240;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
    Vector2dd center(buffer->h / 12, buffer->h / 12);
    double radius = center.x() / 10.0 * 9.0;

    AbstractPainter<RGB24Buffer> painter(buffer);

    for (int pi = 0; pi < 3; pi ++)
    {
        for (int pj = 0; pj < 3; pj ++)
        {
            corecvs::Polygon p;
            int count = pi * 3 + pj + 3;
            for (int i = 0; i < count; i++) {
                p.push_back(Vector2dd::FromPolar((2 * M_PI / count) * i, radius) + center * Vector2dd(pi * 2 + 1, pj * 2 + 1));
            }

           // cout << p << std::endl;

            painter.drawPolygon(p, RGBColor::Blue());

            PolygonSpanIterator it(p);
            for (HLineSpanInt l: it)
            {
                for (Vector2d<int> point : l) {
                    if (buffer->isValidCoord(point)) {
                        buffer->element(point) += RGBColor(90, 90, 0);
                    }
                }
            }
        }
    }

    for (int pi = 0; pi < 3; pi ++)
    {
        for (int pj = 0; pj < 3; pj ++)
        {
            corecvs::Polygon p;
            int count = pi * 3 + pj + 3;
            for (int i = 0; i < count; i++) {
                p.push_back(Vector2dd::FromPolar(-(2 * M_PI / count) * i, radius) + center * Vector2dd(pi * 2 + 1, pj * 2 + 1) + Vector2dd(0.0, buffer->h / 2));
            }

           // cout << p << std::endl;

            painter.drawPolygon(p, RGBColor::Green());

            PolygonSpanIterator it(p);
            for (HLineSpanInt l: it)
            {
//                cout << l << endl;
                for (Vector2d<int> point : l) {
                    if (buffer->isValidCoord(point)) {
                        buffer->element(point) += RGBColor(0, 70, 0);
                    }
                }
            }
        }
    }

    for (int pi = 0; pi < 3; pi ++)
    {
        for (int pj = 0; pj < 3; pj ++)
        {
            corecvs::Polygon p;
            int count = pi * 3 + pj + 3;
            for (int i = 0; i < count; i++) {
                p.push_back(Vector2dd::FromPolar(-(2 * M_PI / count) * i, radius) + center * Vector2dd(pi * 2 + 1, pj * 2 + 1) + Vector2dd(buffer->w / 2, 0.0));
            }

            //cout << p << std::endl;

            painter.drawPolygon(p, RGBColor::Green());

            PolygonPointIterator it(p);
            for (Vector2d<int> point : it) {
                    buffer->element(point) += RGBColor(0, 70, 0);
            }
        }
    }


    BMPLoader().save("polygon1.bmp", buffer);
    delete_safe(buffer);
}
