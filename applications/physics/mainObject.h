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

    /** Owned and manitained by MainObject**/
    vector<SimObject*> objects;

    Vector3dd velocity = Vector3dd::Zero();
    Vector3dd force    = Vector3dd::Zero();
    Vector3dd oldForce = Vector3dd::Zero();
    Vector3dd massCenter = Vector3dd::Zero();
    Vector3dd posCenter = Vector3dd::Zero();
    int frameCounter = 0;

    Quaternion orientation = Quaternion::Identity();

    Quaternion angleVelocity  = Quaternion::Identity();

    /** Mass in kilograms **/
    double systemMass;
    bool countPhysics = false;


    void addForce  (const Vector3dd &force);
    void addImpulse(const Vector3dd &force);
    void setCenterOfMass();
    void setForce(double x, double y, double z);
    void setForce(const Vector3dd &force);
    void tick(double deltaT);

    void addSphere(Vector3dd coords, double radius);

    /**
     * Adds subobject to MainObject and transfers ownership
     **/
    void addObject(SimObject *object);

    virtual ~MainObject();
};



#endif // MAINOBJECT_H
