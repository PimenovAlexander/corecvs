#ifndef PHYSOBJECT_H
#define PHYSOBJECT_H

#include <vector>
#include <bits/stdc++.h>
#include "core/utils/log.h"
#include <core/geometry/mesh3d.h>
#include <cmath>
#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/math/matrix/matrix33.h"
#include "core/geometry/mesh3DDecorated.h"
#include "core/math/affine.h"

/** Temporary usages while we are preparing to move this to the proper place **/
using corecvs::Vector3dd;
using corecvs::Quaternion;

/*
class Force {
public:
    corecvs::Vector3dd force    = corecvs::Vector3dd::Zero();
    //corecvs::Vector3dd position = corecvs::Vector3dd::Zero();
    //corecvs::Vector3dd centerPos = corecvs::Vector3dd::Zero();

    Force() {}

    Force(double fx, double fy, double fz) :
        force(fx, fy, fz)
    {
    }

    Force(Vector3dd force) :
        force(force)
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
*/
    /** Torque in H*m **/
/*
    Vector3dd getM() const
    {
        return (position - centerPos) ^ force;
    }

    Vector3dd getForce() const
    {
        return force;
    }

};
*/

class PhysObject
{
public:
    PhysObject();
    PhysObject(const corecvs::Affine3DQ *coords, const double *m);

    Vector3dd force    = corecvs::Vector3dd::Zero();
    Vector3dd velocity = Vector3dd::Zero();

    int frameCounter = 0;
    Quaternion orientation = Quaternion::Identity();
    Quaternion angularVelocity  = Quaternion::Identity();
    Quaternion angularAcc = Quaternion::Identity();

    /** Main body properties **/
    /** Mass in kilograms **/
    double mass;

    /** Assuming spheroid **/
    corecvs::Matrix33 inertiaTensor = corecvs::Matrix33::Identity();

    /** Main magick **/
    void startTick();
    void addForce   (const Vector3dd &force);
    void addMoment  (const Vector3dd &moment);

    void tick(double deltaT);

    /** state inside tick **/
    Vector3dd getForce() const;
    Vector3dd getMoment() const;
    /* You may want to bring this to separate interface. */
    virtual void addToMesh (corecvs::Mesh3D &mesh);
    virtual void saveMesh  (const std::string &name);
    virtual void drawMesh (corecvs::Mesh3D &mesh);
    virtual void calcMoment();

    /* Get & Set */
    void setPosition(const corecvs::Affine3DQ &pos);
    void setPosition(const corecvs::Vector3dd &pos);
    const corecvs::Affine3DQ getPosAffine() const;
    const corecvs::Vector3dd getPosVector() const;

private:
    corecvs::Affine3DQ position;
    Vector3dd F;
    Vector3dd M;
    //void addImpulse(const Vector3dd &force);
    //void setForce(double x, double y, double z);
    //void setForce(const Vector3dd &force);

};




#endif // PHYSOBJECT_H
