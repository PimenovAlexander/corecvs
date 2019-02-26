#ifndef SIMSPHERE_H
#define SIMSPHERE_H

#include "simobject.h"



class SimSphere : public SimObject
{
public:
    SimSphere();
    void SaveMesh(string name);
    float radius=10;
};

#endif // SIMSPHERE_H
