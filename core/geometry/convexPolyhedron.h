#ifndef CONVEXPOLYHEDRON_H
#define CONVEXPOLYHEDRON_H

#include <vector>
#include "core/geometry/line.h"
#include "core/geometry/plane.h"
#include "core/geometry/axisAlignedBox.h"

namespace corecvs {

class Mesh3D;
using std::vector;

template<typename HalfspaceType, typename VectorType>
class ConvexPolyhedronGeneric
{
public:
    vector<HalfspaceType> faces;

    ConvexPolyhedronGeneric() {}

    bool isInside(const VectorType &p)
    {
        for (HalfspaceType &type: faces )
        {
            if (type.pointWeight(p) < 0)
                return false;
        }
        return true;
    }

    unsigned size() const
    {
		return (unsigned)faces.size();
    }

    VectorType getPoint(int i) const
    {
        return faces[i].projectZeroTo();
    }

    VectorType getNormal(int i) const
    {
        return faces[i].normal();
    }

    friend ostream & operator <<(ostream &out, const ConvexPolyhedronGeneric &polyhedron)
    {
        out << "[";
        for (const HalfspaceType &halfspace : polyhedron.faces) {
            out << halfspace;
        }
        out << "]";
        return out;
    }

    void inset(double dist)
    {
        for (HalfspaceType &type: faces )
        {
            type.last() -= dist * type.normal().l2Metric();
        }
    }

};


class ConvexPolyhedron : public ConvexPolyhedronGeneric<Plane3d, Vector3dd>
{
public:
    ConvexPolyhedron();
    ConvexPolyhedron(const AxisAlignedBox3d &box);
    ConvexPolyhedron(const vector<Vector3dd> &vertices);

    /** Move this out **/
    vector<Vector3dd> vertices;

    bool intersectWith(const Ray3d &ray, double &t1, double &t2)
    {
        return ray.clip<ConvexPolyhedron> (*this, t1, t2);
    }
    static ConvexPolyhedron intersect (const ConvexPolyhedron &poly1, const ConvexPolyhedron &poly2);

    void addToMesh(Mesh3D &mesh);

    /**
     * This would work only for convex mesh input with right order of vertexes in each face
     **/
    ConvexPolyhedron FromMesh(const Mesh3D &input);

};

class ConvexPolygon : public ConvexPolyhedronGeneric< Line2d, Vector2dd>
{
public:   

    void append(const ConvexPolygon &other);
    void simplify();

    static ConvexPolygon intersect (const ConvexPolygon &a1, const ConvexPolygon &a2);
    /*Same as intersect but without simplification */
    static ConvexPolygon merge     (const ConvexPolygon &a1, const ConvexPolygon &a2);
};


} // namespace corecvs


#endif // CONVEXPOLYHEDRON_H
