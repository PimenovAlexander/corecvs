#include "physMainObject.h"


PhysicsMainObject::PhysicsMainObject()
{
    systemMass = 0;
    F = Vector3dd::Zero();

    L_INFO << "default PhysMainObject():created";
}

void PhysicsMainObject::addForce(const corecvs::Vector3dd &_force)
{
    /* objects dont need to know about whole system forces
    for (size_t i = 0; i < objects.size(); ++i)
    {
        objects[i]->addForce(_force);
    }
    */
    F += _force;
}

void PhysicsMainObject::tick(double deltaT)
{
    //L_INFO <<"PhysMainObject::tick should not be called";
}



void PhysicsMainObject::updateCenterOfMass()
{
    for (size_t i = 0; i < objects.size(); ++i)
    {
        massCenter += objects[i]->mass * objects[i]->position();
        systemMass += objects[i]->mass;
    }
    massCenter = massCenter / systemMass;
}

const Vector3dd PhysicsMainObject::getCenterOfMass()
{
    updateCenterOfMass();
    return massCenter;
}

void PhysicsMainObject::setForce(const Vector3dd &_force)
{
    F = _force;
}

const Vector3dd PhysicsMainObject::force()
{
    return F;
}

void PhysicsMainObject::setMomentum(const Vector3dd &_m)
{
    M = _m;
}

const Vector3dd PhysicsMainObject::momentum()
{
    return M;
}

void PhysicsMainObject::setSystemMass(const double m)
{
    systemMass = m;
}

double PhysicsMainObject::getSystemMass() const
{
    return systemMass;
}


void PhysicsMainObject::calcForce()
{
    Vector3dd f = Vector3dd::Zero();
    //L_INFO << "transform: " << transform;
    Vector3dd forceToTransform;
    Vector3dd forceAfterTransform;

    for(size_t i = 0; i < objects.size(); ++i)
    {        
        objects[i]->calcForce();
        Affine3DQ transform = x;
        forceToTransform = objects[i]->force();
        forceAfterTransform = transform.rotor * forceToTransform;
        f += forceAfterTransform;
        //L_INFO << "added force: " << forceAfterTransform << "; forceToTransform: " << forceToTransform;
    }
    //L_INFO << "final force to add to PhysMainObject: " << f;
    addForce(f);
}

void PhysicsMainObject::calcMoment()
{
    Vector3dd m = Vector3dd::Zero();
    Affine3DQ transform = x;
    for(size_t i = 0; i < objects.size(); ++i)
    {
        objects[i]->calcMoment();

        m += transform.rotor * objects[i]->moment();
    }

    setMomentum(m);
}

void PhysicsMainObject::setPosCenter(const Vector3dd &_pos)
{
    x.shift = _pos;
}

Vector3dd PhysicsMainObject::getPosCenter() const
{
    return x.shift;
}

Quaternion PhysicsMainObject::orientation() const
{
    return x.rotor;
}

void PhysicsMainObject::setOrientation(const Quaternion &orientation)
{
    x.rotor = orientation;
}


void PhysicsMainObject::startTick()
{
    F = Vector3dd::Zero();
    M = Vector3dd::Zero();
    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i]->startTick();       
    }
}

void PhysicsMainObject::addObject(PhysicsObject *object)
{
    objects.push_back(object);
    updateCenterOfMass();
}

PhysicsMainObject::~PhysicsMainObject()
{
    objects.clear();
}
