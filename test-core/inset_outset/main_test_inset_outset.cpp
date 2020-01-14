/**
 * \file main_test_inset_outset.cpp
 * \brief This is the main file for the test inset/outset of polyline
 *
 * \date Nov 23, 2019
 * \author Ivan Kylchik
 *
 * \ingroup autotest
 */

#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/geometry/insetOutset.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/fileformats/bmpLoader.h"

void drawSegments(AbstractPainter<RGB24Buffer> *painter, std::vector<Segment2d> &segments, RGBColor color) {
    auto it = segments.begin();
    while (it != segments.end()) {
        painter->drawPath(PointPath{it->a, it->b}, color);
        ++it;
    }
}

void drawPolygon(const Polygon &p, int offset, const std::string &fileName) {
    std::vector<Segment2d> inset = shiftPolygon(p, -offset);
    std::vector<Segment2d> outset = shiftPolygon(p, offset);

    RGB24Buffer buffer(1000, 1000);
    AbstractPainter<RGB24Buffer> painter(&buffer);
    painter.drawPolygon(p, RGBColor::Blue());
    drawSegments(&painter, inset, RGBColor::Green());
    drawSegments(&painter, outset, RGBColor::Red());
    BMPLoader().save(fileName + ".bmp", &buffer);
}

void drawPointPath(const PointPath &p, int offset, const std::string &fileName) {
    std::vector<Segment2d> inset = shiftPointPath(p, -offset);
    std::vector<Segment2d> outset = shiftPointPath(p, offset);

    RGB24Buffer buffer(1000, 1000);
    AbstractPainter<RGB24Buffer> painter(&buffer);
    painter.drawPath(p, RGBColor::Blue());
    drawSegments(&painter, inset, RGBColor::Green());
    drawSegments(&painter, outset, RGBColor::Red());
    BMPLoader().save(fileName + ".bmp", &buffer);
}

TEST(inset_outset, arrow) {
    Polygon p{
            Vector2dd(120, 300),
            Vector2dd(400, 120),
            Vector2dd(290, 300),
            Vector2dd(400, 480)
    };

    drawPolygon(p, 10, "arrow");

}

TEST(inset_outset, smallObj) {
    Polygon p{
            Vector2dd(60, 105),
            Vector2dd(50, 100),
            Vector2dd(100, 100),
            Vector2dd(100, 105)
    };

    drawPolygon(p, 10, "smallObj");
}

TEST(inset_outset, hourglass) {
    Polygon p{
            Vector2dd(150, 300),
            Vector2dd(100, 100),
            Vector2dd(200, 100),
            Vector2dd(150, 300),
            Vector2dd(200, 500),
            Vector2dd(100, 500)
    };

    drawPolygon(p, 10, "hourglass");
}

TEST(inset_outset, selfIntersecting) {
    Polygon p{
            Vector2dd(50, 100),
            Vector2dd(200, 80),
            Vector2dd(170, 175),
            Vector2dd(230, 160),
            Vector2dd(60, 160)
    };

    drawPolygon(p, 10, "selfIntersecting");
}


TEST(inset_outset, simplePath) {
    PointPath p{
            Vector2dd(50, 100),
            Vector2dd(200, 80),
            Vector2dd(170, 175)
    };

    drawPointPath(p, 10, "simplePath");
}

TEST(inset_outset, overlapLines) {
    PointPath p{
            Vector2dd(50, 100),
            Vector2dd(100, 100),
            Vector2dd(100, 150),
            Vector2dd(100, 200)
    };

    drawPointPath(p, 10, "overlapLines");
}

TEST(inset_outset, moreComplicatedPath) {
    PointPath p{
            Vector2dd(50, 300),
            Vector2dd(200, 300),
            Vector2dd(70, 100),
            Vector2dd(70, 400)
    };

    drawPointPath(p, 10, "moreComplicatedPath");
}

TEST(inset_outset, star) {
    Polygon p{
            Vector2dd(140, 120),
            Vector2dd(200, 300),
            Vector2dd(260, 120),
            Vector2dd(100, 230),
            Vector2dd(300, 230)
    };

    drawPolygon(p, 10, "star");
}

TEST(inset_outset, singlePoint) {
    Polygon p{
            Vector2dd(100, 100)
    };

    drawPolygon(p, 10, "singlePoint");
}

TEST(inset_outset, zeroPoint) {
    Polygon p{};

    drawPolygon(p, 10, "zeroPoint");
}