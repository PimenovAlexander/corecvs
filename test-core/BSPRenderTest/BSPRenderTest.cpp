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
    RGB24Buffer *buffer1 = new RGB24Buffer(h, w, RGBColor::Black());

    p.push_back(Vector2dd(59, 59));

    p.push_back(Vector2dd(299, 59));
    p.push_back(Vector2dd(239, 239));
    p.push_back(Vector2dd(479, 299));
    p.push_back(Vector2dd(479, 59));

    p.push_back(Vector2dd(659, 59));
    p.push_back(Vector2dd(659, 479));
    p.push_back(Vector2dd(539, 659));

    p.push_back(Vector2dd(479, 659));
    p.push_back(Vector2dd(479, 479));
    p.push_back(Vector2dd(299, 419));
    p.push_back(Vector2dd(299, 599));

    p.push_back(Vector2dd(59, 659));

    AbstractPainter<RGB24Buffer> painter(buffer);
    painter.drawPolygon(p, RGBColor::White());

    BMPLoader().save("levelPolygon.bmp", buffer);

//    Ray2d ray(Vector2dd(100, 719), Vector2dd(200, 0));
//    cout << "ray.getStart() = " << ray.getStart() << endl;
//    cout << "ray.getEnd() = " << ray.getEnd() << endl;
//    buffer1->drawLine(ray.getStart(), ray.getEnd(), RGBColor::Yellow());
//    double t1, t2;
//    ray.clip<Polygon>(p, t1, t2);

//    Vector2dd p1 = ray.getPoint(t1);
//    Vector2dd p2 = ray.getPoint(t2);

//    cout << "t1 = " << t1 << endl;
//    cout << "t2 = " << t2 << endl;
//    cout << "p1 = " << p1 << endl;
//    cout << "p2 = " << p2 << endl;

//    buffer1->drawLine(p1, p2, t1 > t2 ? RGBColor::Red() : RGBColor::Green());
//    AbstractPainter<RGB24Buffer> painter1(buffer1);
//    painter1.drawPolygon(p, RGBColor::White());

//    BMPLoader().save("levelPolygon1.bmp", buffer1);

//    Ray2d ray = p.getRay(2);
//    Vector2dd p1 = ray.getStart();
//    Vector2dd p2 = ray.getEnd();
//    printf("Start: %f, %f\n"
//           "End: %f, %f\n",
//           p1.x(), p1.y(),
//           p2.x(), p2.y());
//    buffer->drawLine(p1, p2, RGBColor::Red());
//    BMPLoader().save("levelPolygon1.bmp", buffer);

    delete_safe(buffer);
    delete_safe(buffer1);
}
