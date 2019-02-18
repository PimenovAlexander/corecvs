#include "simobject.h"
#include <bits/stdc++.h>
#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"
void SimObject::AddForce(Vector3d<float> _force)
{

}
void SimObject::AddImpulse(Vector3d<float> _force)
{

}
void SimObject::SetForce(Vector3d<float> _force)
{
   force= Vector3d<float>(_force);
}

void SimObject::SetForce(float x,float y, float z)
{
   force= Vector3d<float>(x,y,z);
}


SimObject::SimObject()
{

}
