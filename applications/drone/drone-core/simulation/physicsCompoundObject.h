#ifndef PHYSMAINOBJECT_H
#define PHYSMAINOBJECT_H



#include <list>
#include <iostream>

#include "physicsObject.h"
#include "physicsSphere.h"

#include <bits/stl_list.h>
#include <bits/stdc++.h>

/* This class is not needed */
class PhysicsMainObject
{
public:
    corecvs::Vector3dd F          = corecvs::Vector3dd::Zero();
    corecvs::Vector3dd oldF       = corecvs::Vector3dd::Zero();
    corecvs::Vector3dd massCenter = corecvs::Vector3dd::Zero();
    corecvs::Vector3dd M          = corecvs::Vector3dd::Zero();
    double systemMass;

public:
    PhysicsMainObject();

    vector<PhysicsObject *> objects;

    /** Position and orientation **/
    corecvs::Affine3DQ  x  = corecvs::Affine3DQ::Identity();

    /** Speed **/
    corecvs::Vector3dd  v  = corecvs::Vector3dd::Zero();

    /** Angular Speed **/
    corecvs::Vector3dd  w  = corecvs::Vector3dd::Zero();

    /** Angular Acceleration **/
    corecvs::Vector3dd tau = corecvs::Vector3dd::Zero();

    /** Mass in kilograms **/
    bool countPhysics = false;
    corecvs::Matrix33 inertiaTensor = corecvs::Matrix33::Identity();

    void addForce  (const corecvs::Vector3dd &force);
    void addImpulse(const corecvs::Vector3dd &force);
    void updateCenterOfMass();
    const corecvs::Vector3dd getCenterOfMass();

    /** Get & Set **/
    void setForce   (const corecvs::Vector3dd &_force);
    void setMomentum(const corecvs::Vector3dd &_m);

    const corecvs::Vector3dd force();
    const corecvs::Vector3dd momentum();

    void setSystemMass(const double m);
    double getSystemMass() const;

    void calcForce();
    void calcMoment();

    corecvs::Quaternion orientation() const;
    void setOrientation(const corecvs::Quaternion &orientation);

    void setPosCenter(const corecvs::Vector3dd &_pos);
    corecvs::Vector3dd getPosCenter() const;

    void startTick();

    virtual void tick(double deltaT);

    /**
     * Adds subobject to MainObject ownership
     **/
    void addObject(PhysicsObject *object);

    virtual ~PhysicsMainObject();
 };



#endif // PHYSMAINOBJECT_H
