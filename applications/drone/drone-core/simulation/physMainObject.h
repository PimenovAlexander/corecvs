#ifndef PHYSMAINOBJECT_H
#define PHYSMAINOBJECT_H



#include <list>
#include <iostream>

#include "physObject.h"
#include "physSphere.h"

#include <bits/stl_list.h>
#include <bits/stdc++.h>

/* This class is not needed */
class PhysicsMainObject
{
public:
    Vector3dd F    = Vector3dd::Zero();
    Vector3dd oldF = Vector3dd::Zero();
    Vector3dd massCenter = Vector3dd::Zero();
    Vector3dd M = Vector3dd::Zero();
    double systemMass;

public:
    PhysicsMainObject();

    vector<PhysicsObject *> objects;

    /** Position and orientation **/
    Affine3DQ  x  = Affine3DQ::Identity();

    /** Speed **/
    Vector3dd  v  = Vector3dd::Zero();

    /** Angular Speed **/
    Vector3dd  w  = Vector3dd::Zero();

    /** Angular Acceleration **/
    Vector3dd tau = Vector3dd::Zero();

    /** Mass in kilograms **/
    bool countPhysics = false;
    corecvs::Matrix33 inertiaTensor = corecvs::Matrix33::Identity();

    void addForce  (const Vector3dd &force);
    void addImpulse(const Vector3dd &force);
    void updateCenterOfMass();
    const Vector3dd getCenterOfMass();

    /** Get & Set **/
    void setForce   (const Vector3dd &_force);
    void setMomentum(const Vector3dd &_m);

    const Vector3dd force();
    const Vector3dd momentum();

    void setSystemMass(const double m);
    double getSystemMass() const;

    void calcForce();
    void calcMoment();

    Quaternion orientation() const;
    void setOrientation(const Quaternion &orientation);

    void setPosCenter(const Vector3dd &_pos);
    Vector3dd getPosCenter() const;

    void startTick();

    virtual void tick(double deltaT);

    /**
     * Adds subobject to MainObject ownership
     **/
    void addObject(PhysicsObject *object);

    virtual ~PhysicsMainObject();
 };



#endif // PHYSMAINOBJECT_H
