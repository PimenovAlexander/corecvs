#ifndef PHYSMAINOBJECT_H
#define PHYSMAINOBJECT_H

#include "bits/stdc++.h"
#include "list"
#include "iostream"
#include "physObject.h"
#include "physSphere.h"

#include <bits/stl_list.h>

class PhysMainObject
{
private:
    Vector3dd force    = Vector3dd::Zero();
    Vector3dd oldForce = Vector3dd::Zero();
    Vector3dd massCenter = Vector3dd::Zero();
    Vector3dd posCenter;
    Vector3dd momentum = Vector3dd::Zero();
    double systemMass;

public:
    PhysMainObject();

    /** Owned and manitained by PhysMainObject**/
    vector<PhysObject*> objects;

    Vector3dd velocity = Vector3dd::Zero();
    int frameCounter = 0;

    Quaternion orientation = Quaternion::Identity();

    Quaternion angularVelocity  = Quaternion::Identity();

    /** Mass in kilograms **/
    bool countPhysics = false;
    corecvs::Matrix33 inertiaTensor = corecvs::Matrix33::Identity();

    void addForce  (const Vector3dd &force);
    void addImpulse(const Vector3dd &force);
    void calcCenterOfMass();
    const Vector3dd getCenterOfMass();

    /** Get & Set **/
    void setForce(const Vector3dd &_force);
    const Vector3dd getForce();
    void setMomentum(const Vector3dd &_m);
    const Vector3dd getMomentum();
    void setSystemMass(const double m);
    double getSystemMass() const;

    void calcForce();
    void calcMoment();

    void setPosCenter(const Vector3dd &_pos);
    const Vector3dd getPosCenter();

    void startTick();

    virtual void tick(double deltaT);

    void addSphere(Vector3dd coords, double radius);
    void addSphere(Vector3dd coords, double radius, corecvs::RGBColor);

    /**
     * Adds subobject to MainObject and transfers ownership
     **/
    void addObject(PhysObject *object);

    virtual ~PhysMainObject();
 };



#endif // PHYSMAINOBJECT_H
