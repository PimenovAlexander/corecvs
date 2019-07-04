#ifndef PHYSSPHERE_H
#define PHYSSPHERE_H

#include "physObject.h"
using namespace corecvs;
class PhysSphere : public PhysObject
{
public:
    PhysSphere();
    PhysSphere(corecvs::Affine3DQ *pos, double *r, double *m);

    /*Get & Set*/
    void setPos(const Vector3dd &pos);

    void addToMesh (corecvs::Mesh3D &mesh) override;
    void saveMesh(const std::string &name) override;
    void drawMesh(corecvs::Mesh3D &mesh) override;
    void calcMoment() override;
    void calcForce() override;


    double radius;
};

#endif // PHYSSPHERE_H
