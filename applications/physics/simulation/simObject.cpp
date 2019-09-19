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
    F += force.getForce();
    M += force.getM();
}

void SimObject::addMoment(const Vector3dd &moment)
{
    M += moment;
}

void SimObject::tick(double deltaT)
{
    /** KILLME **/
    double radius = 0.050;
    double motorMass = 0.01;
    double frameSize = 0.088;
    double centerMass = 0.08;
    double inertialMomentX = 2.0 / 5.0 * centerMass * pow(radius, 2) + 2 * motorMass * pow(frameSize / 2, 2);
    double inertialMomentY = 2.0 / 5.0 * centerMass * pow(radius, 2) + 2 * motorMass * pow(frameSize / 2, 2);
    double inertialMomentZ = 2.0 / 5.0 * centerMass * pow(radius, 2) + 4 * motorMass * pow(frameSize / 2, 2);
    inertiaTensor = Matrix33(inertialMomentX, 0, 0,
                             0, inertialMomentY, 0,
                             0, 0, inertialMomentZ);

    position += velocity * deltaT;
    velocity += (F / mass) * deltaT;

    /* We should carefully use inertiaTensor here. It seems like it changes with the frame of reference */
    Vector3dd W = inertiaTensor.inv() * M;
    Quaternion angularAcceleration = Quaternion::Rotation(W, W.l2Metric());

    Quaternion q = orientation;
    orientation     = Quaternion::pow(angularVelocity    , deltaT) ^ orientation;

    //orientation.printAxisAndAngle();
    angularVelocity = Quaternion::pow(angularAcceleration, deltaT) ^ angularVelocity;
    L_INFO<<"Delta orient: "<<abs(orientation.getAngle()-q.getAngle());

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
