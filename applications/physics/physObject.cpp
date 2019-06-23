#include "physObject.h"

#include <bits/stdc++.h>

#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"

using namespace std;
using namespace corecvs;


void PhysObject::startTick()
{
    F = Vector3dd::Zero();
    M = Vector3dd::Zero();
}

void PhysObject::addForce(const Vector3dd &force)
{
    F += force;
    //M += force.getM();
}

void PhysObject::addMoment(const Vector3dd &moment)
{
    M += moment;
}

void PhysObject::calcMoment()
{
    M = force * getPosVector();
    SYNC_PRINT(("PhysObject::calcMoment() was called"));
}

void PhysObject::tick(double deltaT)
{

}

const corecvs::Affine3DQ PhysObject::getPosAffine() const
{
    return position;
}

const Vector3dd PhysObject::getPosVector() const
{
    return position.shift;
}

Vector3dd PhysObject::getForce() const
{
    return F;
}

Vector3dd PhysObject::getMoment() const
{
    return M;
}

void PhysObject::addToMesh(Mesh3D &mesh)
{
    SYNC_PRINT(("Don't know how to do this"));
}

void PhysObject::drawMesh(Mesh3D &mesh)
{
    SYNC_PRINT(("PhysObject can't draw mesh"));
}

void PhysObject::setPosition(const Affine3DQ &pos)
{
    this->position = pos;
}

void PhysObject::setPosition(const Vector3dd &pos)
{
    this->position = Affine3DQ(pos);
}

PhysObject::PhysObject()
{
    position = Affine3DQ(Vector3dd(1,1,1));
    mass = 1;

    F = Vector3dd::Zero();
    M = Vector3dd::Zero();

    L_INFO << "default PhysObject():created";
}

PhysObject::PhysObject(const Affine3DQ *coords, const double *m)
{
    position = Affine3DQ(*coords);
    mass = *m;

    F = Vector3dd::Zero();
    M = Vector3dd::Zero();

    L_INFO << "PhysObject():created with " << mass << " kg mass and at " << position << " position";
}

void PhysObject::saveMesh(const std::string &/*name*/)
{
    cout << "PhysObject::saveMesh(): not here" << endl;
}
