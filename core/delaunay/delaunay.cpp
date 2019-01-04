/*
 * \file delaunay.h
 * \brief delaunay triangulation
 *
 * \ingroup cppcorefiles
 * \date Dec 22, 2018
 * \author Spirin Egor
 */

#include "core/delaunay/delaunay.h"

#include "core/math/mathUtils.h"
#include "delaunay.h"

namespace corecvs {

DelaunayTriangulation::DelaunayTriangulation(const vector<Vector2dd>& points) :
        points_(points)
{
    removeSamePoint();
    if (points_.size() < 3) {
        throw std::invalid_argument("need at least 3 points for triangulation");
    }
    buildTriangulation();
}

void DelaunayTriangulation::getPoints(vector<Vector2dd>* points) {
    for (auto& point : points_) {
        points->emplace_back(point);
    }
}

void DelaunayTriangulation::getTriangulation(vector<Triangle2dd>* triangles) {
    for (auto& tri : triangles_) {
        triangles->emplace_back(tri.first);
    }
}

void DelaunayTriangulation::removeSamePoint() {
    std::vector<Vector2dd> cur_points = std::move(points_);
    points_.clear();
    for (auto& point : cur_points) {
        bool flag = true;
        for (auto& contain_point : points_) {
            if (contain_point == point) {
                flag = false;
                break;
            }
        }
        if (flag) {
            points_.emplace_back(point);
        }
    }
}

/**
 * Build delaunay triangulation using Bowyer-Watson algorithm
 * https://en.wikipedia.org/wiki/Bowyer–Watson_algorithm
 **/
void DelaunayTriangulation::buildTriangulation() {
    triangles_.clear();

    // add super-triangle, which cover all points

    // It is posible to use Rectangled here with Rectangled::extendToFit(), this could save two passes
    auto minMaxX = std::minmax_element(points_.begin(), points_.end(),
                                       [](const Vector2dd& a, const Vector2dd& b) {
                                           return a.x() < b.x();
                                       });

    auto minMaxY = std::minmax_element(points_.begin(), points_.end(),
                                       [](const Vector2dd& a, const Vector2dd& b) {
                                           return a.y() < b.y();
                                       });

    // adding offsets for nonchecking, that point lies on side of supertriangle
    auto minX = (*minMaxX.first)[0] - OFFSET, maxX = (*minMaxX.second)[0] + OFFSET,
         minY = (*minMaxY.first)[1] - OFFSET, maxY = (*minMaxY.second)[1] + OFFSET;

    // form vertexes of supertriangle
    Vector2dd p1(maxX + 1.0 * (maxY - minY) / tan(degToRad(60)), minY);
    Vector2dd p2(minX - 1.0 * (maxY - minY) / tan(degToRad(60)), minY);
    Vector2dd p3((minX + maxX) / 2, maxY + 1.0 * (minX + maxX) / 2 / tan(degToRad(30)));

    triangles_.emplace_back(Triangle2dd(p1, p2, p3), true);
    // iterate over all points and insert each into triangulation
    for (auto& point : points_) {
        addPointToTriangulation(point);
    }
    // remove triangles with point from supertriangle
    std::vector<Vector2dd> vertex = {p1, p2, p3};
    triangles_.erase(std::remove_if(triangles_.begin(), triangles_.end(),
                                    [vertex](const std::pair<Triangle2dd, bool>& a) {
                                        for (auto& point : vertex) {
                                            if (point == a.first.p1() ||
                                                point == a.first.p2() ||
                                                point == a.first.p3()) {
                                                return true;
                                            }
                                        }
                                        return false;
                                    }), triangles_.end());
}

void DelaunayTriangulation::addPointToTriangulation(const corecvs::Vector2dd& point)
{
    std::vector<std::pair<std::pair<Vector2dd, Vector2dd>, bool>> polygon;
    // filter triangles by containing point in circumcircle
    for (auto& tri : triangles_) {
        if (pointInsideCircumcircle(point, tri.first)) {
            polygon.emplace_back(std::make_pair(tri.first.p1(), tri.first.p2()), true);
            polygon.emplace_back(std::make_pair(tri.first.p2(), tri.first.p3()), true);
            polygon.emplace_back(std::make_pair(tri.first.p3(), tri.first.p1()), true);
            tri.second = false;
        } else {
            tri.second = true;
        }
    }
    triangles_.erase(remove_if(triangles_.begin(), triangles_.end(),
                               [](const std::pair<Triangle2dd, bool>& a) {
                                   return !a.second;
                               }), triangles_.end());

    for (size_t i = 0; i < polygon.size(); i++) {
        for (size_t j = i + 1; j < polygon.size(); j++) {
            // check that this segments are equal
            if (almostEqualSegments(polygon[i].first, polygon[j].first)) {
                polygon[i].second = false;
                polygon[j].second = false;
            }
        }
    }
    // create new triangles
    for (auto& seg : polygon) {
        if (seg.second) {
            triangles_.emplace_back(Triangle2dd(seg.first.first, seg.first.second, point), true);
        }
    }
}

/**
 *  calculate radius of circumcircle
 *   R = (a * b * c) / (4 * S)
 **/
bool DelaunayTriangulation::pointInsideCircumcircle(
        const corecvs::Vector2dd& point,
        const corecvs::Triangle2dd& triangle)
{

    auto a = triangle.side1().getLength(),
         b = triangle.side2().getLength(),
         c = triangle.side3().getLength();

    auto p = (a + b + c) / 2;
    auto S = sqrt(p * (p - a) * (p - b) * (p - c));
    auto R = a * b * c / (4 * S);
    /* Could be more stability here if we carefully keep R squared. */
    return (point - getCircumcircleCenter(triangle)).l2Metric() <= R;
}


bool DelaunayTriangulation::almostEqualSegments(
        const std::pair<Vector2dd, Vector2dd>& a,
        const std::pair<Vector2dd, Vector2dd>& b) {
    if (a.first.notTooFar(b.first, EPSILON) && a.second.notTooFar(b.second, EPSILON)) {
        return true;
    } else if (a.second.notTooFar(b.first, EPSILON) && a.first.notTooFar(b.second, EPSILON)) {
        return true;
    } else {
        return false;
    }
}


Vector2dd DelaunayTriangulation::getCircumcircleCenter(const Triangle2dd& triangle)
{
    // solution of  system: (x - a)^2 + (y - b)^2 = r^2
    // This is basically intersection of two perpendicular to chord
    // Direct solution may be faster
    // To get more stability it is possible to use https://en.wikipedia.org/wiki/Circumscribed_circle#Cartesian_coordinates approach

    auto x1 = triangle.p1().x(), y1 = triangle.p1().y();
    auto x2 = triangle.p2().x(), y2 = triangle.p2().y();
    auto x3 = triangle.p3().x(), y3 = triangle.p3().y();
    Matrix22 A(
            2 * (x2 - x1), 2 * (y2 - y1),
            2 * (x3 - x1), 2 * (y3 - y1)
    );
    Vector2dd B(
            y2 * y2 + x2 * x2 - y1 * y1 - x1 * x1,
            y3 * y3 + x3 * x3 - y1 * y1 - x1 * x1
    );
    return Matrix22::solve(A, B);
}

} // namespace corecvs
