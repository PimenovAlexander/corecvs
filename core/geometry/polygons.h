#ifndef POLYGONS_H_
#define POLYGONS_H_

/**
 * \file polygons.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 14, 2010
 * \author alexander
 */


#include <vector>
#include <algorithm>

#include "vector3d.h"
#include "generated/axisAlignedBoxParameters.h"
#include "line.h"

namespace corecvs {

using std::vector;

template<typename PointType>
class GenericTriangle
{
public:
    PointType p[3];

    PointType &p1() {return p[0];}
    PointType &p2() {return p[1];}
    PointType &p3() {return p[2];}

    const PointType &p1() const {return p[0];}
    const PointType &p2() const {return p[1];}
    const PointType &p3() const {return p[2];}

    GenericTriangle() {}

    GenericTriangle(const PointType _p1, const PointType _p2, const PointType _p3)
    {
        p1() = _p1;
        p2() = _p2;
        p3() = _p3;
    }


    Plane3d getPlane() const
    {
        return Plane3d::FromPoints(p1(), p2(), p3());
    }

    Vector3dd getNormal() const
    {
        return Plane3d::NormalFromPoints(p1(), p2(), p3());
    }

    /** NOTE: This could swap the normal **/
    void sortByY() {
        if (p1().y() > p2().y()) std::swap(p1(), p2());
        if (p2().y() > p3().y()) std::swap(p2(), p3());
        if (p1().y() > p2().y()) std::swap(p1(), p2());
    }

};

typedef GenericTriangle<Vector3d<int32_t> > Triangle32;
typedef GenericTriangle<Vector3d<double> > Triangle3dd;
typedef GenericTriangle<Vector2d<double> > Triangle2dd;


class PointPath : public vector<Vector2dd>
{
public:
    PointPath(){}

    PointPath(int len) : vector<Vector2dd>(len)
    {}
};

/**
 *  Polygon
 **/
class Polygon : public PointPath
{
public:
    Polygon(){}

    Polygon(const Vector2dd *points, int len) : PointPath(len)
    {
        for (unsigned i = 0; i < size(); i++) {
           this->operator[](i) = points[i];
        }
    }

    int isInside(const Vector2dd &point);
    bool isConvex(bool *direction = NULL);

    Vector2dd getPoint (int i) const
    {
        return operator [](i);
    }

    Vector2dd getNormal(int i) const
    {
        Vector2dd r1 = operator []( i              );
        Vector2dd r2 = operator []((i + 1) % size());

        return (r2 - r1).rightNormal();
    }

    //bool clipRay(const Ray2d &ray, double &t1, double &t2);

};

/* So far we only support convex polygon*/
class PolygonSpanIterator
{
public:
    const Polygon &polygon;

    double currentY;
    double startX;
    double endX;


    vector<int> sortedIndex;
    unsigned currentIndex;

    PolygonSpanIterator(const Polygon &polygon) : polygon(polygon)
    {
        sortedIndex.reserve(polygon.size());
        for (unsigned i = 0; i < polygon.size(); i++)
            sortedIndex.push_back(i);

        std::sort(sortedIndex.begin(), sortedIndex.end(), [=](int a, int b) { return polygon[a].y() > polygon[b].y(); });
        currentIndex = 0;
        currentY = polygon[sortedIndex[0]].y();
        startX   = polygon[sortedIndex[0]].x();
        endX    =  polygon[sortedIndex[0]].x();

    }

    bool step()
    {
        SYNC_PRINT(("PolygonSpanIterator::step(): called\n"));
        if (currentIndex >= sortedIndex.size())
            return false;
    }

    void getSpan(double &y, double &x1, double &x2)
    {
        y = currentY;
        x1 = 0;
        x2 = 0;
    }





};


} //namespace corecvs
#endif /* POLYGONS_H_ */

