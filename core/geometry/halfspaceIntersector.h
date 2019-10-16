#ifndef HALFSPACEINTERSECTOR_H
#define HALFSPACEINTERSECTOR_H

#include "core/geometry/polygons.h"
#include "core/geometry/convexQuickHull.h"
#include "core/geometry/projectiveConvexQuickHull.h"

namespace corecvs {

class HalfspaceIntersector
{
public:
    HalfspaceIntersector();
#if 0
    // Unfinished
    static Polygon FromConvexPolygon(const ConvexPolygon &polygon);
#endif

    // Use this
    static ConvexPolygon Simplify(const ConvexPolygon &polygon);
    static ProjectivePolygon FromConvexPolygonP (const ConvexPolygon &polygon);


    static ConvexQuickHull::HullFaces FromConvexPolyhedron(const ConvexPolyhedron &polygon);
    static ConvexPolyhedron FromConvexPolyhedronCP(const ConvexPolyhedron &polyhedron);
};

} // namespace corecvs

#endif // HALFSPACEINTERSECTOR_H
