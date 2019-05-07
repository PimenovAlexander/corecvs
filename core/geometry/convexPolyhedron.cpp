#include "core/geometry/convexPolyhedron.h"
#include "core/geometry/halfspaceIntersector.h"
#include "mesh3d.h"

namespace corecvs {

ConvexPolyhedron::ConvexPolyhedron()
{
}

ConvexPolyhedron::ConvexPolyhedron(const AxisAlignedBox3d &box)
{
    faces.push_back(Plane3d::FromNormalAndPoint(Vector3dd( 0,  0,  1), box.low()));
    faces.push_back(Plane3d::FromNormalAndPoint(Vector3dd( 0,  1,  0), box.low()));
    faces.push_back(Plane3d::FromNormalAndPoint(Vector3dd( 1,  0,  0), box.low()));

    faces.push_back(Plane3d::FromNormalAndPoint(Vector3dd( 0,  0, -1), box.high()));
    faces.push_back(Plane3d::FromNormalAndPoint(Vector3dd( 0, -1,  0), box.high()));
    faces.push_back(Plane3d::FromNormalAndPoint(Vector3dd(-1,  0,  0), box.high()));

}

ConvexPolyhedron::ConvexPolyhedron(const vector<Vector3dd> &vertices)
{
    for(const Vector3dd &vertex: vertices)
    {
        this->vertices.push_back(vertex);
    }
    ConvexQuickHull::HullFaces hullFaces = ConvexQuickHull::quickHull(vertices);
    for(ConvexQuickHull::HullFace hullFace: hullFaces)
    {
        this->faces.push_back(hullFace.getPlane());
        /*Plane3d plane(Plane3d::FromPoints(hullFace.plane.p1(), hullFace.plane.p2(), hullFace.plane.p3()));
        if(plane.a()<0) plane.flipNormal();
        else if(plane.a()==0)
        {
            if(plane.b()<0) plane.flipNormal();
            else if(plane.b()==0)
            {
                if(plane.c()<0) plane.flipNormal();
            }
        }
        this->faces.push_back(plane);*/
    }
}

void ConvexPolygon::append(const ConvexPolygon &other)
{
    faces.insert(faces.begin(), other.faces.begin(), other.faces.end());
}

void ConvexPolygon::simplify()
{
    this->faces = HalfspaceIntersector::FromConvexPolygonCP(*this).faces;
}

/*
void ConvexPolygon::intersectWith(const ConvexPolygon &other)
{

}
*/

std::vector<Vector3dd> ConvexPolygon::toDualPoints()
{
    std::vector<Vector3dd> dual;
    dual.reserve(faces.size());
    for (const Line2d &line : faces)
    {
        dual.push_back(line.toDualP());
    }
    return dual;
}

ConvexPolygon ConvexPolygon::merge(const ConvexPolygon &a1, const ConvexPolygon &a2)
{
    ConvexPolygon toReturn;
    toReturn = a1;
    toReturn.append(a2);
    return toReturn;
}

ConvexPolygon ConvexPolygon::intersect(const ConvexPolygon &a1, const ConvexPolygon &a2)
{
    ConvexPolygon toReturn = merge(a1, a2);
    toReturn.simplify();
    return toReturn;
}


ConvexPolyhedron ConvexPolyhedron::intersect(const ConvexPolyhedron &poly1, const ConvexPolyhedron &poly2)
{
    ConvexPolyhedron toReturn;
    for (const Plane3d &face : poly1.faces)
    {
        toReturn.faces.push_back(face);       
    }
    for (const Plane3d &face : poly2.faces)
    {
        toReturn.faces.push_back(face);
    }
    return HalfspaceIntersector::FromConvexPolyhedronCP(toReturn);
}

void ConvexPolyhedron::addToMesh(Mesh3D &mesh)
{
    ConvexQuickHull::HullFaces faces = ConvexQuickHull::quickHull(this->vertices);
    faces.addToMesh(mesh);
}

ConvexPolyhedron ConvexPolyhedron::FromMesh(const Mesh3D &input)
{
    ConvexPolyhedron toReturn;
    for (size_t f = 0; f < input.faces.size(); f++)
    {
        toReturn.faces.push_back(input.getFaceAsPlane(f));
    }
    return toReturn;
}

} // namespace corecvs
