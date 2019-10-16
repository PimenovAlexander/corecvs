#ifndef CONVEXDEBUG_H
#define CONVEXDEBUG_H

#include <core/geometry/mesh3d.h>
#include <core/geometry/polygons.h>

using namespace corecvs;

class ConvexDebug
{
public:
    ConvexDebug();

    static bool GiftWrap(ProjectivePolygon &points, ProjectivePolygon &output, Mesh3D *debug);

    static int InitailPoint(ProjectivePolygon &planes, Mesh3D *debug);
    static int InitailPoint1(ProjectivePolygon &planes, Mesh3D *debug);

    static void drawProjectiveLines(Mesh3D &mesh, const Vector3dd &lined, const Circle2d &circle2d);


};

#endif // CONVEXDEBUG_H
