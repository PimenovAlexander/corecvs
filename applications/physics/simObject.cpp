#include "simObject.h"

#include <bits/stdc++.h>

#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"

using namespace std;
using namespace corecvs;


void SimObject::startTick()
{
    F = Vector3dd::Zero();
    M = Vector3dd::Zero();
}

void SimObject::addForce(const Force &force)
{
    F += force.netForce();
    M += force.getM();
}

void SimObject::addMoment(const Vector3dd &moment)
{
    M += moment;
}

void SimObject::tick(double deltaT)
{    
    position += velocity * deltaT;
    velocity += (F / mass) * deltaT;

    /* We should carefully use inertiaTensor here. It seems like it changes with the frame of reference */

    Vector3dd W = inertiaTensor.inv() * M;
    Quaternion angularAcceleration = Quaternion::Rotation(W, W.l2Metric());

    orientation     = Quaternion::pow(angularVelocity    , deltaT) ^ orientation;
    angularVelocity = Quaternion::pow(angularAcceleration, deltaT) ^ angularVelocity;


}

void SimObject::addToMesh(Mesh3D &mesh)
{
    SYNC_PRINT(("Don't know how to do this"));
}

void SimObject::setCoords(const Vector3dd &c)
{
    this->position = c;
}

SimObject::SimObject()
{
    cout << "SimObject():created" << endl;
}



void SimObject::saveMesh(const std::string &/*name*/)
{
    cout << "SimObject::saveMesh(): not here" << endl;
}
