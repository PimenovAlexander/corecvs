#ifndef SIMSPHERE_H
#define SIMSPHERE_H

#include "simObject.h"


class SimSphere : public SimObject
{
public:
    SimSphere();
    virtual void saveMesh(const std::string &name) override;

    double radius = 10.0;
};

#endif // SIMSPHERE_H
