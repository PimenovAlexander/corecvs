#ifndef SIMOBJECT_H
#define SIMOBJECT_H
#include <bits/stdc++.h>
#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"
class SimObject
{
public:
    SimObject();
    Vector3d<float> coords;
    Vector3d<float> angle;
    Vector3d<float> force;
    Vector3d<float> velocity;
    float mass;
    bool CountPhysics=false;

    void AddForce(Vector3d<float> force);
    void AddImpulse(Vector3d<float> force);
    void SetForce(float x, float y, float z);
    void SetForce(Vector3d<float> force);
    void Tick(double DeltaT);
};

#endif // SIMOBJECT_H
