#include "core/geometry/convexPolyhedron.h"
#include "core/geometry/halfspaceIntersector.h"

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

void ConvexPolygon::append(const ConvexPolygon &other)
{
    faces.insert(faces.begin(), other.faces.begin(), other.faces.end());
}

void ConvexPolygon::simplify()
{
    this->faces = HalfspaceIntersector::FromConvexPolygonCP(*this).faces;
}

void ConvexPolygon::intersectWith(const ConvexPolygon &other)
{

}

ConvexPolygon intersect(const ConvexPolygon &a1, const ConvexPolygon &a2)
{
    ConvexPolygon toReturn;
    toReturn = a1;
    toReturn.append(a2);
    toReturn.simplify();
    return toReturn;
}

} // namespace corecvs
