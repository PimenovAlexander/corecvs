#ifndef SIMSPHERE_H
#define SIMSPHERE_H

#include "simObject.h"


class SimSphere : public SimObject
{
public:
    SimSphere();

    virtual void addToMesh (corecvs::Mesh3D &mesh) override;
    virtual void saveMesh(const std::string &name) override;

    double radius = 10.0;
    corecvs::RGBColor mainColour=corecvs::RGBColor::Red();  // color of sphere

    void drawMesh(corecvs::Vector3dd position, double radius, corecvs::RGBColor color);
    SimSphere(Vector3dd c, double r);
    SimSphere(Vector3dd c, double r, corecvs::RGBColor col);
};

#endif // SIMSPHERE_H
