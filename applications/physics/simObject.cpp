#include "simObject.h"
#include <bits/stdc++.h>
#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"
void SimObject::addForce(Vector3d<float> _force)
{
    force+=_force;
}
void SimObject::addImpulse(Vector3d<float> _force)
{

}
void SimObject::setForce(Vector3d<float> _force)
{
    force= Vector3d<float>(_force);
}

void SimObject::setForce(float x,float y, float z)
{
    force= Vector3d<float>(x,y,z);
}

void SimObject::tick(double DeltaT)
{
    float dT=(float)DeltaT;
    coords+=dT*(velocity+dT*(force+oldForce)/mass/4);
    velocity+=force/mass;
    oldForce=force;
}

SimObject::SimObject()
{
    cout<<"created"<<endl;
}

void SimObject::saveMesh(string name)
{
    cout<<"not here"<<endl;
}
