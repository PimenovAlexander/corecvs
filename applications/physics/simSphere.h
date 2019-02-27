#ifndef SIMSPHERE_H
#define SIMSPHERE_H

#include "simObject.h"



class SimSphere : public SimObject
{
public:
    SimSphere();
    void saveMesh(string name);
    float radius=10;
};

#endif // SIMSPHERE_H
