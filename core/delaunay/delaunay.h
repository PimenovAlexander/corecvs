/*
 * \file delaunay.h
 * \brief delaunay triangulation
 *
 * \ingroup cppcorefiles
 * \date Dec 22, 2018
 * \author Spirin Egor
 */

#ifndef _DELAUNAY_H
#define _DELAUNAY_H

#include <vector>

#include "core/math/vector/vector2d.h"
#include "core/geometry/polygons.h"

namespace corecvs {

class DelaunayTriangulation {
 public:
    explicit DelaunayTriangulation(const std::vector<Vector2dd>& points);

    void getPoints(std::vector<Vector2dd>* points);

    void getTriangulation(std::vector<Triangle2dd>* triangles);

    static bool pointInsideCircumcircle(const Vector2dd& point, const Triangle2dd& triangle);

    void buildTriangulation();

 private:
    const double EPSILON = 1e-5;
    const int OFFSET = 100;

    std::vector<Vector2dd> points_;
    std::vector<std::pair<Triangle2dd, bool>> triangles_;

    void removeSamePoint();

    void addPointToTriangulation(const Vector2dd& point);

    static Vector2dd getCircumcircleCenter(const Triangle2dd& triangle);

    bool almostEqualSegments(const Segment2d& seg1, const Segment2d& seg2);

};

}  // namespace corecvs


#endif  // _DELAUNAY_H
