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

};

#endif // CONVEXDEBUG_H
