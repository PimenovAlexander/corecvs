#ifndef MOTOR_H
#define MOTOR_H

#include <copterInputs.h>
#include <vector>

#include <core/cameracalibration/cameraModel.h>
#include "core/cameracalibration/calibrationDrawHelpers.h"
#include "core/geometry/mesh3d.h"
#include "core/math/affine.h"
#include "core/math/vector/vector3d.h"

#include "physSphere.h"

class Motor : public PhysSphere
{
public:
    /**
     *  By design this is an encapsulated motor class, it knows nothing about how it is mounted
     **/
    /* Configuration */
    bool cw;
    double maxForce = 9.8 / 3; /* Each motor is capable of just lifing itself */

    double motorWidth; /**< in m **/
    double motorHeight; /**< in m **/

    std::string name = "-";
    corecvs::RGBColor color = corecvs::RGBColor::Red();

    /* State */
    double pwm = 0.0; /**< 0..1 **/
    double phi = degToRad(32);
    virtual void calcMoment() override;
    virtual void drawMesh(corecvs::Mesh3D &mesh) override;
    Vector3dd getForce();
    void calcForce();
    Vector3dd getForceTransformed(const Affine3DQ &T);
    Vector3dd calcMotorMoment();

    /* UI not owned. Need to be reworked. Could be reused for hardcoded solution */
    Mesh3D *motorMesh = NULL;
    Mesh3D *propMesh  = NULL;
    Motor();
};

#endif // MOTOR_H
