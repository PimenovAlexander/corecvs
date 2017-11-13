#include "core/geometry/convexPolyhedron.h"

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

} // namespace corecvs
