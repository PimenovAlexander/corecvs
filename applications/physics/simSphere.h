#ifndef SIMSPHERE_H
#define SIMSPHERE_H

#include "simObject.h"


class SimSphere : public SimObject
{
public:
    SimSphere();
    virtual void saveMesh(const std::string &name) override;

    double radius = 10.0;
    void drawMesh(corecvs::Vector3dd coords, double radius);
    SimSphere(Vector3dd c, double r);
};

#endif // SIMSPHERE_H
