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
        return faces.size();
    }

    Vector3dd getPoint(int i) const
    {
        return faces[i].projectZeroTo();
    }

    Vector3dd getNormal(int i) const
    {
        return faces[i].normal();
    }

};

} // namespace corecvs


#endif // CONVEXPOLYHEDRON_H
