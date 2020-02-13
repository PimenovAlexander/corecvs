#include "physicsObject.h"

#include <bits/stdc++.h>

#include "core/utils/utils.h"
#include "core/geometry/mesh/mesh3d.h"
#include "core/geometry/mesh/mesh3DDecorated.h"

using namespace std;
using namespace corecvs;


void PhysicsObject::startTick()
{
    F = Vector3dd::Zero();
    M = Vector3dd::Zero();
}

void PhysicsObject::addForce(const Vector3dd &force)
{
    F += force;
    //M += force.getM();
}

void PhysicsObject::addMoment(const Vector3dd &moment)
{
    M += moment;
}

Affine3DQ PhysicsObject::affine() const
{
    return x;
}

Vector3dd PhysicsObject::position() const
{
    return x.shift;
}

Vector3dd PhysicsObject::force() const
{
    return F;
}

Vector3dd PhysicsObject::moment() const
{
    return M;
}


void PhysicsObject::setPosition(const Affine3DQ &pos)
{
    this->x = pos;
}

void PhysicsObject::setPosition(const Vector3dd &pos)
{
    this->x = Affine3DQ(pos);
}

PhysicsObject::PhysicsObject()
{
    x = Affine3DQ(Vector3dd(1.0, 1.0, 1.0));
    mass = 1.0;

    F = Vector3dd::Zero();
    M = Vector3dd::Zero();

    //L_INFO << "default PhysObject():created";
}

PhysicsObject::PhysicsObject(const Affine3DQ &coords, const double &m)
{
    x = Affine3DQ(coords);
    mass = m;

    F = Vector3dd::Zero();
    M = Vector3dd::Zero();

    //L_INFO << "PhysObject():created with " << mass << " kg mass and at " << position << " position";
}
