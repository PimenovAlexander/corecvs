#include "mainObject.h"

MainObject::MainObject()
{
}

void MainObject::tick(double deltaT)
{
    Vector3dd deltaPos = posCenter;
    for (int i = 0; i < objects.size(); ++i)
    {
        systemMass += objects[i]->mass;
    }
    //posCenter += deltaT * (velocity + deltaT * (force + oldForce) / systemMass / 4.0 );
    //velocity += ((force + oldForce) * deltaT) / (2 * systemMass);
    //oldForce = force;

    velocity=Vector3dd(0.1, 0.1, 0.1);
    posCenter += velocity * deltaT;
    deltaPos = posCenter - deltaPos;


    for (int i = 0; i < objects.size(); ++i)
    {
        //objects[i]->coords += deltaPos;             //If uncomment ui thread will crash after pressing start virtual mode ????????????????

        //objects[i]->setCoords(objects[i]->coords+deltaPos);  //Also ui thread crash

        objects[i]->setCoords(posCenter);             //Normalno
    }

    for (int i = 0; i < spheres.size(); ++i)
    {
        spheres[i].coords=objects[i]->coords;
    }
}

void MainObject::setCenterOfMass()
{
    for (int i = 0; i < objects.size(); ++i)
    {
        massCenter += objects[i]->mass * objects[i]->coords;
        systemMass += objects[i]->mass;
    }
    massCenter = massCenter / systemMass;
}

void MainObject::addObject(SimObject object)
{
    objects.push_back(&object);
    setCenterOfMass();
}


void MainObject::addSphere(Vector3dd coords, double radius)
{
    int num = spheres.size();
    spheres.push_back(SimSphere(coords,radius));
    objects.push_back(&spheres[num]);
    setCenterOfMass();
}
