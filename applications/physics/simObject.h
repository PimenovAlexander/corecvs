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
    Vector3d<float> coords = Vector3d<float>(1,1,1);
    Vector3d<float> velocity = Vector3d<float>(0,0,0);
    Vector3d<float> force = Vector3d<float>(0,0,0);
    Vector3d<float> oldForce = Vector3d<float>(0,0,0);

    int FrameCounter=0;
    Quaternion qPos = Quaternion(0,0,0,0);
    Quaternion qVx = Quaternion(1,0,0,0);
    Quaternion qVy = Quaternion(0,1,0,0);
    Quaternion qVz = Quaternion(0,0,1,0);

    float mass = 1;                                    //kg
    bool countPhysics=false;

    void addForce(Vector3d<float> force);
    void addImpulse(Vector3d<float> force);
    void setForce(float x, float y, float z);
    void setForce(Vector3d<float> force);
    void tick(double DeltaT);
    virtual void saveMesh(string name);
};

#endif // SIMOBJECT_H
