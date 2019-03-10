#ifndef MAINOBJECT_H
#define MAINOBJECT_H
#include "bits/stdc++.h"
#include "list"
#include "iostream"
#include "simObject.h"
#include "simSphere.h"

#include <bits/stl_list.h>

class MainObject
{
public:
    MainObject();
    vector<SimObject*> objects;
    vector<SimSphere> spheres;
    Vector3dd velocity = Vector3dd::Zero();
    Vector3dd force    = Vector3dd::Zero();
    Vector3dd oldForce = Vector3dd::Zero();
    Vector3dd massCenter = Vector3dd::Zero();
    Vector3dd posCenter = Vector3dd::Zero();
    int frameCounter = 0;

    Quaternion orientation = Quaternion(0.0, 0.0, 0.0, 0.0);

    Quaternion angleVelocity  = Quaternion(0.0 ,0.0 ,0.0 ,0.0);

    /** Mass in kilograms **/
    double systemMass;
    bool countPhysics=false;


    void addForce  (const Vector3dd &force);
    void addImpulse(const Vector3dd &force);
    void setCenterOfMass();
    void setForce(double x, double y, double z);
    void setForce(const Vector3dd &force);
    void tick(double deltaT);
    void addObject(SimObject object);
    void addSphere(Vector3dd coords, double radius);
};

#endif // MAINOBJECT_H
