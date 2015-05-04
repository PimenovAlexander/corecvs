/**
 * \file polygons.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 14, 2010
 * \author alexander
 */

#ifndef POLYGONS_H_
#define POLYGONS_H_

#include <vector>

#include "vector3d.h"
#include "generated/axisAlignedBoxParameters.h"
#include "line.h"

namespace corecvs {

using std::vector;

template<typename ElementType>
class GenericTriangle
{
public:
    Vector3d<ElementType> p1;
    Vector3d<ElementType> p2;
    Vector3d<ElementType> p3;
};

typedef GenericTriangle<int32_t> Triangle;


class PointPath : public vector<Vector2dd>
{
public:
    PointPath(){}

    PointPath(int len) : vector<Vector2dd>(len)
    {}
};

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


} //namespace corecvs
#endif /* POLYGONS_H_ */

