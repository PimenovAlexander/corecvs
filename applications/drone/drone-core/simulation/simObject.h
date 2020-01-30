#ifndef SIMOBJECT_H
#define SIMOBJECT_H

#include <vector>
#include <bits/stdc++.h>
#include "core/utils/log.h"
#include <core/geometry/mesh/mesh3d.h>
#include <cmath>
#include "core/utils/utils.h"
#include "core/geometry/mesh/mesh3d.h"
#include "core/math/matrix/matrix33.h"
#include "core/geometry/mesh/mesh3DDecorated.h"

/** Temporary usages while we are preparing to move this to the proper place **/
using corecvs::Vector3dd;
using corecvs::Quaternion;

class Force {
public:
    corecvs::Vector3dd force    = corecvs::Vector3dd::Zero();
    corecvs::Vector3dd position = corecvs::Vector3dd::Zero();
    corecvs::Vector3dd centerPos = corecvs::Vector3dd::Zero();

    Force() {}

    Force(double fx, double fy, double fz) :
        force(fx, fy, fz), position(Vector3dd::Zero())
    {
    }

    Force(Vector3dd force, Vector3dd pos = Vector3dd::Zero()) :
        force(force), position(pos)
    {
    }

    void transform(const corecvs::Affine3DQ &T)
    {
        force    = T.rotor * force;
        position = T * position;

        //L_INFO<<"Force Pos: "<< position;
        //L_INFO<<"Center Pos: "<<centerPos;
    }

    Force transformed(corecvs::Affine3DQ &T) const
    {
        Force toReturn = *this;
        toReturn.transform(T);
        return toReturn;
    }

    Force transformed(const corecvs::Affine3DQ &T, corecvs::Vector3dd &cPos)
    {
        centerPos = cPos;
        Force toReturn = *this;
        toReturn.transform(T);
        return toReturn;
    }

    /** Torque in H*m **/
    /* BUG: not working properly, deltaR should be not from beginning of coordinats */
    Vector3dd getM() const
    {
        return (position - centerPos) ^ force;
    }

/*
    Vector3dd getM(const Vector3dd &objectPos) const
    {
        return (objectPos-position) ^ force;
    }
*/
    Vector3dd getForce() const
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
    Vector3dd vec_W;
    //Vector3dd force    = Vector3dd::Zero();
    //Vector3dd oldForce = Vector3dd::Zero();

    int frameCounter = 0;


    Quaternion orientation = Quaternion::Identity();
    Quaternion angularVelocity  = Quaternion::Identity();
    Quaternion angularAcc = Quaternion::Identity();

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
