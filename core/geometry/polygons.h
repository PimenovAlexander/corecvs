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
    PointPath(){};

    PointPath(int len) : vector<Vector2dd>(len)
    {};
};

class Poligon : public PointPath
{
public:

    Poligon(){};

    Poligon(const Vector2dd *points, int len) : PointPath(len)
    {
        for (unsigned i = 0; i < size(); i++) {
           this->operator[](i) = points[i];
        }
    }

    int isInside(const Vector2dd &point);
};


} //namespace corecvs
#endif /* POLYGONS_H_ */

