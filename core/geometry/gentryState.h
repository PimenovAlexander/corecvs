#ifndef GENTRYSTATE_H
#define GENTRYSTATE_H

#include "math/vector/vector3d.h"
#include "geometry/line.h"
#include "cameracalibration/cameraModel.h"

using namespace corecvs;

class GentryState
{
public:
    CameraModel camera;
    Plane3d laserPlane;

    Vector3dd trinagulate(Vector2dd &pixel);
    Vector2dd project(Vector3dd &point);

    GentryState();
};

#endif // GENTRYSTATE_H
