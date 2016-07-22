#ifndef CONVEXPOLYHEDRON_H
#define CONVEXPOLYHEDRON_H

#include <vector>
#include "line.h"
#include "axisAlignedBox.h"

namespace corecvs {

using std::vector;

class ConvexPolyhedron
{
public:
    vector<Plane3d> faces;
    ConvexPolyhedron();

    ConvexPolyhedron(const AxisAlignedBox3d &box);


    bool isInside();

    unsigned size() const
    {
		return (unsigned)faces.size();
    }

    Vector3dd getPoint(int i) const
    {
        return faces[i].projectZeroTo();
    }

    Vector3dd getNormal(int i) const
    {
        return faces[i].normal();
    }

    bool intersectWith(const Ray3d &ray, double &t1, double &t2)
    {
        return ray.clip<ConvexPolyhedron> (*this, t1, t2);
    }

};

} // namespace corecvs


#endif // CONVEXPOLYHEDRON_H
