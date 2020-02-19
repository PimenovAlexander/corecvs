#ifndef POINTOBSERVATION_H
#define POINTOBSERVATION_H

#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"

namespace corecvs {

using std::istream;
using std::ostream;

struct PointObservation
{
    Vector3dd point;
    Vector2dd projection;

    PointObservation(
            const Vector3dd &_point      = Vector3dd(0),
            const Vector2dd &_projection = Vector2dd(0)
    ) : point(_point),
        projection(_projection)
    {}


    inline double &x() { return point.x(); }
    inline double &y() { return point.y(); }
    inline double &z() { return point.z(); }

    inline double &u() { return projection.x(); }
    inline double &v() { return projection.y(); }

    inline const double &x() const { return point.x(); }
    inline const double &y() const { return point.y(); }
    inline const double &z() const { return point.z(); }

    inline const double &u() const { return projection.x(); }
    inline const double &v() const { return projection.y(); }

    template<class VisitorType>
        void accept(VisitorType &visitor)
        {
            visitor.visit(point, Vector3dd(), "point3d");
            visitor.visit(projection, Vector2dd(), "projection");
        }


    friend std::ostream & operator <<(std::ostream &out, const PointObservation &observation)
    {
        out << "(" << observation.point << " -> " << observation.projection << ")";
        return out;
    }

};

} // namespace corecvs

#endif // POINTOBSERVATION_H
