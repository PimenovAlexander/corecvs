#ifndef PHYSICS_SPHERE_H
#define PHYSICS_SPHERE_H

#include "physicsObject.h"

class PhysicsSphere : public MaterialObject
{
public:
    PhysicsSphere();
    PhysicsSphere(const corecvs::Affine3DQ &pos, double r, double m);

    /*Get & Set*/
    void setPos(const corecvs::Vector3dd &pos);

    void addToMesh (corecvs::Mesh3D &mesh) override;
    void drawMesh  (corecvs::Mesh3D &mesh) override;

    void saveMesh  (const std::string &name) override;
    void calcMoment() override;
    void calcForce() override;

    corecvs::RGBColor color = corecvs::RGBColor::Red();
    double radius;
};

#endif // PHYSSPHERE_H
