#ifndef SIMOBJECT_H
#define SIMOBJECT_H

#include <bits/stdc++.h>

#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"

/** Temporary usages while we are preparing to move this to the proper place **/
using corecvs::Vector3dd;
using corecvs::Quaternion;



class SimObject
{
public:
    SimObject();
    Vector3dd coords   = Vector3dd(1,1,1);
    Vector3dd velocity = Vector3dd::Zero();
    Vector3dd force    = Vector3dd::Zero();
    Vector3dd oldForce = Vector3dd::Zero();

    int frameCounter = 0;

    Quaternion orientation = Quaternion(0.0, 0.0, 0.0, 0.0);

    Quaternion angleVelocity  = Quaternion(0.0 ,0.0 ,0.0 ,0.0);


    /** Mass in kilograms **/
    double mass = 1;
    bool countPhysics=false;

    void addForce  (const Vector3dd &force);
    void addImpulse(const Vector3dd &force);

    void setForce(double x, double y, double z);
    void setForce(const Vector3dd &force);
    void tick(double deltaT);

    virtual void saveMesh(const std::string &name);
    void setCoords(Vector3dd c);
};

#endif // SIMOBJECT_H
