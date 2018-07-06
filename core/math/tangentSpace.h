#ifndef TANGENTSPACE_H
#define TANGENTSPACE_H

#include "vector/vector2d.h"
#include "vector/vector3d.h"
#include "quaternion.h"

namespace corecvs {

class TangentSpaceSO3
{
public:
    Vector3dd data;

    /* Math pseudonims */
    double & a() { return data.x(); }
    const double & a() const { return data.x(); }

    double & b() { return data.y(); }
    const double & b() const { return data.y(); }

    double & c() { return data.z(); }
    const double & c() const { return data.z(); }


    Matrix33 toMatrix()
    {
        return Matrix33::CrossProductLeft(data);
    }

    double theta() { return data.l2Metric(); }

    Matrix33   expM();
    Quaternion exp();

    TangentSpaceSO3();
};

} // namespace corecvs

#endif // TANGENTSPACE_H
