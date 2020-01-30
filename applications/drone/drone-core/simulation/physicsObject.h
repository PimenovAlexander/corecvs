#ifndef PHYSOBJECT_H
#define PHYSOBJECT_H

#include <cmath>
#include <vector>
#include <bits/stdc++.h>

#include "core/utils/log.h"
#include "core/geometry/mesh/mesh3d.h"

#include "core/utils/utils.h"
#include "core/geometry/mesh/mesh3d.h"
#include "core/math/matrix/matrix33.h"
#include "core/geometry/mesh/mesh3DDecorated.h"
#include "core/math/affine.h"

class PhysicsObject
{
public:
    PhysicsObject();
    PhysicsObject(const corecvs::Affine3DQ &coords, const double &m);

    /**
     *   Current object position and orientation
     **/
    corecvs::Affine3DQ x = corecvs::Affine3DQ::Identity();

    /**
     *   Current object speed
     **/
    corecvs::Vector3dd v = corecvs::Vector3dd::Zero();

    /*Do we need this*/
    corecvs::Quaternion r   = corecvs::Quaternion::Identity();
    corecvs::Quaternion w   = corecvs::Quaternion::Identity();
    corecvs::Quaternion tau = corecvs::Quaternion::Identity();

    corecvs::Vector3dd F;
    corecvs::Vector3dd M;

    /** Main body properties **/
    /** Mass in kilograms **/
    double mass;

    /** Assuming spheroid **/
    corecvs::Matrix33 inertiaTensor = corecvs::Matrix33::Identity();

    /** Main magick **/
    void startTick();
    void addForce   (const corecvs::Vector3dd &force);
    void addMoment  (const corecvs::Vector3dd &moment);

    /** state inside tick **/
    virtual void calcMoment() = 0;
    virtual void calcForce() = 0;

    /** Get & Set **/
    corecvs::Vector3dd force() const;
    corecvs::Vector3dd moment() const;

    void setPosition(const corecvs::Affine3DQ &pos);
    void setPosition(const corecvs::Vector3dd &pos);
    corecvs::Affine3DQ affine() const;
    corecvs::Vector3dd position() const;

    virtual ~PhysicsObject() {}
};

class MaterialObject : public PhysicsObject
{
public:
    MaterialObject() {}
    MaterialObject(const corecvs::Affine3DQ &coords, const double &m):
        PhysicsObject(coords, m)
    {}


    /* You may want to bring this to separate interface. */
    virtual void addToMesh (corecvs::Mesh3D &mesh) = 0;
    virtual void saveMesh  (const std::string &name) = 0;
    virtual void drawMesh (corecvs::Mesh3D &mesh) = 0;

};

#endif // PHYSOBJECT_H
