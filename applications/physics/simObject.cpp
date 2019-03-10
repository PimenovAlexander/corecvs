#include "simObject.h"

#include <bits/stdc++.h>

#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"

using namespace std;
using namespace corecvs;

void SimObject::addForce(const corecvs::Vector3dd &_force)
{
    force += _force;
}

void SimObject::addImpulse(const Vector3dd &_force)
{
}

void SimObject::setForce(const Vector3dd &_force)
{
    force = Vector3dd(_force);
}

void SimObject::setForce(double x,double y, double z)
{
    force = Vector3dd(x, y, z);
}

void SimObject::tick(double deltaT)
{    

}

void SimObject::addToMesh(Mesh3D &mesh)
{
    SYNC_PRINT(("Don't know how to do this"));
}

void SimObject::setCoords(Vector3dd c)
{
    this->coords = c;
}

SimObject::SimObject()
{
    cout << "SimObject():created" << endl;
}

void SimObject::saveMesh(const std::string &/*name*/)
{
    cout << "SimObject::saveMesh(): not here" << endl;
}
