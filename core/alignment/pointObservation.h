#ifndef POINTOBSERVATION_H
#define POINTOBSERVATION_H

#include "vector2d.h"
#include "vector3d.h"

namespace corecvs {

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


    inline double &x()
    {
        return point.x();
    }

    inline double &y()
    {
        return point.y();
    }

    inline double &z()
    {
        return point.z();
    }

    inline double &u()
    {
        return projection.x();
    }

    inline double &v()
    {
        return projection.y();
    }

    template<class VisitorType>
        void accept(VisitorType &visitor)
        {
            visitor.visit(point, Vector3dd(), "point3d");
            visitor.visit(projection, Vector2dd(), "projection");
        }


    friend ostream & operator <<(ostream &out, const PointObservation &observation)
    {
        out << "(" << observation.point << " -> " << observation.projection << ")";
        return out;
    }

};

} // namespace corecvs

#endif // POINTOBSERVATION_H
