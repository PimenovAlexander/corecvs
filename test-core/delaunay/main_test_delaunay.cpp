/**
 * \file main_test_delaunay.cpp
 * \brief This is the main file for the test delaunay triangulation
 *
 * \date Dec 22, 2018
 * \author Spirin Egor
 */

#include <random>
#include <vector>
#include <iostream>
#include "gtest/gtest.h"

#include "core/delaunay/delaunay.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/bufferFactory.h"


using namespace corecvs;

TEST(delaunay, 3_points)
{
    vector<Vector2dd> points;
    points.emplace_back(0, 0);
    points.emplace_back(1, 0);
    points.emplace_back(0, 1);
    DelaunayTriangulation delaunay(points);

    vector<Triangle2dd> triangles;
    delaunay.getTriangulation(&triangles);
    CORE_ASSERT_TRUE(triangles.size() == 1, "Unable to triangulate trivial case");
}

TEST(delaunay, 3_points_with_duplicates)
{
    vector<corecvs::Vector2dd> points;
    points.emplace_back(0, 0);
    points.emplace_back(0, 0);
    points.emplace_back(1, 0);
    points.emplace_back(1, 0);
    points.emplace_back(0, 1);

    DelaunayTriangulation delaunay(points);

    vector<Triangle2dd> triangles;
    delaunay.getTriangulation(&triangles);
    EXPECT_EQ(1, triangles.size());
    points.clear();
    delaunay.getPoints(&points);
    EXPECT_EQ(3, points.size());
}

TEST(delaunay, less_than_3_points) {
    vector<corecvs::Vector2dd> points;
    points.emplace_back(0, 0);
    points.emplace_back(1, 0);
    try {
        auto delaunay = corecvs::DelaunayTriangulation(points);
    } catch (std::invalid_argument& e) {
        int correct = strcmp("need at least 3 points for triangulation", e.what());
        EXPECT_EQ(0, correct);
    }
}

TEST(delaunay, check_point_inside_circumcircle)
{
    // circumcircle with center in (-2, 3) and radius 10
    Triangle2dd triangle({4, -5}, {8, 3}, {-8, 11});

    EXPECT_TRUE (DelaunayTriangulation::pointInsideCircumcircle({-2, 5}, triangle));
    EXPECT_TRUE (DelaunayTriangulation::pointInsideCircumcircle({4, -5}, triangle));
    EXPECT_FALSE(DelaunayTriangulation::pointInsideCircumcircle({100, 100}, triangle));
}

TEST(delaunay, 100_random_point)
{

    /* Unit test are kept deterministic, otherwize it would be a pain to debug */
    //std::random_device rd;
    std::mt19937 gen;

    RGB24Buffer image(300, 300, RGBColor::White());
    std::uniform_real_distribution<double> dis(0.0, image.w - 1);

    vector<Vector2dd> points;
    points.reserve(100);
    for (int i = 0; i < 100; i++) {
        points.emplace_back(dis(gen), dis(gen));
    }
    DelaunayTriangulation delaunay(points);
    vector<Triangle2dd> triangles;
    delaunay.getTriangulation(&triangles);

    cout << "Triangulation finished. Triangulation size:"  << triangles.size() << endl;

    bool correct = true;
    for (auto& tri : triangles) {
        std::vector<Vector2dd> vertex = {tri.p1(), tri.p2(), tri.p3()};
        for (auto& point : points) {
            bool tri_vertex = false;
            for (auto& v : vertex) {
                tri_vertex |= (v == point);
            }
            if (tri_vertex) {
                continue;
            }
            correct &= !(DelaunayTriangulation::pointInsideCircumcircle(point, tri));
        }
    }

    for (Triangle2dd& tri : triangles) {
        image.drawTriangle(tri, RGBColor::Blue(), 0);
    }

    for (Vector2dd& point : points) {
        image.drawCrosshare3(point, RGBColor::Red());
    }


    BufferFactory::getInstance()->saveRGB24Bitmap(&image, "delaunay.bmp");

    CORE_ASSERT_TRUE(correct, "Random point trianglation fails");
}
