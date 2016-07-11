#ifndef GENTRYSTATE_H
#define GENTRYSTATE_H

#include "vector3d.h"
#include "line.h"
#include "calibrationCamera.h"

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
