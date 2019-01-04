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

std::random_device rd;
std::mt19937 gen(rd());

TEST(delaunay, 3_points) {
    vector<corecvs::Vector2dd> points;
    points.emplace_back(0, 0);
    points.emplace_back(1, 0);
    points.emplace_back(0, 1);
    auto delaunay = corecvs::DelaunayTriangulation(points);
    vector<corecvs::Triangle2dd> triangles;
    delaunay.GetTriangulation(&triangles);
    EXPECT_EQ(1, triangles.size());
}

TEST(delaunay, 3_points_with_duplicates) {
    vector<corecvs::Vector2dd> points;
    points.emplace_back(0, 0);
    points.emplace_back(0, 0);
    points.emplace_back(1, 0);
    points.emplace_back(1, 0);
    points.emplace_back(0, 1);
    auto delaunay = corecvs::DelaunayTriangulation(points);
    vector<corecvs::Triangle2dd> triangles;
    delaunay.GetTriangulation(&triangles);
    EXPECT_EQ(1, triangles.size());
    points.clear();
    delaunay.GetPoints(&points);
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

TEST(delaunay, check_point_inside_circumcircle) {
    // circumcircle with center in (-2, 3) and radius 10
    corecvs::Triangle2dd triangle({4, -5}, {8, 3}, {-8, 11});
    EXPECT_TRUE(corecvs::DelaunayTriangulation::PointInsideCircumcircle({-2, 5}, triangle));
    EXPECT_TRUE(corecvs::DelaunayTriangulation::PointInsideCircumcircle({4, -5}, triangle));
    EXPECT_FALSE(corecvs::DelaunayTriangulation::PointInsideCircumcircle({100, 100}, triangle));
}

TEST(delaunay, 100_random_point) {
    std::uniform_int_distribution<> dis(-100, 100);
    vector<corecvs::Vector2dd> points;
    points.reserve(100);
    for (int i = 0; i < 100; ++i) {
        points.emplace_back(dis(gen), dis(gen));
    }
    auto delaunay = corecvs::DelaunayTriangulation(points);
    vector<corecvs::Triangle2dd> triangles;
    delaunay.GetTriangulation(&triangles);

    bool correct = true;
    for (auto& tri : triangles) {
        std::vector<corecvs::Vector2dd> vertex = {tri.p1(), tri.p2(), tri.p3()};
        for (auto& point : points) {
            bool tri_vertex = false;
            for (auto& v : vertex) {
                tri_vertex |= (v == point);
            }
            if (tri_vertex) {
                continue;
            }
            correct &= !(corecvs::DelaunayTriangulation::PointInsideCircumcircle(point, tri));
        }
    }
    EXPECT_TRUE(correct);
}

TEST(delaunay, 3_points_in_line) {
    vector<corecvs::Vector2dd> points;
    points.emplace_back(0, 0);
    points.emplace_back(0, 1);
    points.emplace_back(0, 2);
    auto delaunay = corecvs::DelaunayTriangulation(points);
    vector<corecvs::Triangle2dd> triangles;
    delaunay.GetTriangulation(&triangles);
    EXPECT_EQ(0, triangles.size());
}

TEST(delaunay, 3_points_in_line_2) {
    vector<corecvs::Vector2dd> points;
    points.emplace_back(0, 0);
    points.emplace_back(0, 1);
    points.emplace_back(0, 2);
    points.emplace_back(1, 1);
    auto delaunay = corecvs::DelaunayTriangulation(points);
    vector<corecvs::Triangle2dd> triangles;
    delaunay.GetTriangulation(&triangles);
    EXPECT_EQ(2, triangles.size());
}

TEST(delaunay, empty_input) {
    try {
        auto delaunay = corecvs::DelaunayTriangulation({});
    } catch (std::invalid_argument& e) {
        int correct = strcmp("need at least 3 points for triangulation", e.what());
        EXPECT_EQ(0, correct);
    }
}
