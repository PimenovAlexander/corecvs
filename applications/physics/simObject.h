#ifndef SIMOBJECT_H
#define SIMOBJECT_H

#include <bits/stdc++.h>

#include <core/geometry/mesh3d.h>

#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/math/matrix/matrix33.h"
#include "core/geometry/mesh3DDecorated.h"

/** Temporary usages while we are preparing to move this to the proper place **/
using corecvs::Vector3dd;
using corecvs::Quaternion;

class Force {
public:
    corecvs::Vector3dd force;
    corecvs::Vector3dd position;

    Force(Vector3dd force, Vector3dd pos = Vector3dd::Zero()) :
        force(force), position(pos)
    {
    }

    void transform(const corecvs::Affine3DQ &T)
    {
        force    = T.rotor * force;
        position = T * position;
    }

    Force transformed(corecvs::Affine3DQ &T) const
    {
        Force toReturn = *this;
        toReturn.transform(T);
        return toReturn;
    }

    /** Torque in H*m **/
    Vector3dd getM() const
    {
        return position ^ force;
    }

    Vector3dd netForce() const
    {
        return force;
    }

};

class SimObject
{
public:
    SimObject();

    SimObject(const Vector3dd &coords) :
        position(coords)
    {}

    Vector3dd position   = Vector3dd(1,1,1);
    Vector3dd velocity = Vector3dd::Zero();

    //Vector3dd force    = Vector3dd::Zero();
    //Vector3dd oldForce = Vector3dd::Zero();

    int frameCounter = 0;


    Quaternion orientation = Quaternion::Identity();
    Quaternion angularVelocity  = Quaternion::Identity();


    /** Main body properties **/
    /** Mass in kilograms **/
    double mass = 1;
    /** Assuming spheroid **/
    corecvs::Matrix33 inertiaTensor = corecvs::Matrix33::Identity();


    /** Main magick **/
    void startTick();
    void addForce   (const Force &force);
    void addMoment  (const Vector3dd &moment);
    void tick(double deltaT);

    /** state inside tick **/
private:
    Vector3dd F;
    Vector3dd M;


    //void addImpulse(const Vector3dd &force);
    //void setForce(double x, double y, double z);
    //void setForce(const Vector3dd &force);
public:

    /* You may want to bring this to separate interface. */
    virtual void addToMesh (corecvs::Mesh3D &mesh);
    virtual void saveMesh  (const std::string &name);

    /* Setters */
    void setCoords(const Vector3dd &c);
};

#endif // SIMOBJECT_H
