#ifndef QUAD_H
#define QUAD_H

#include <copterInputs.h>
#include <vector>

#include <core/cameracalibration/cameraModel.h>
#include "core/cameracalibration/calibrationDrawHelpers.h"
#include "core/geometry/mesh3d.h"
#include "core/math/affine.h"
#include "core/math/vector/vector3d.h"

#include "simObject.h"

class PID
{
public:
    double P,I,D;
    double prevError = 0.0;
    double sumOfError = 0.0;

    PID(double p, double i , double d);

};

class Motor
{
public:
    /**
     * This needs to be stored outside of the Motor class.
     * I'm just too lasy
     *
     *  By design this is an encapsulated motor class, it knows nothing about how it is mounted
     *
     **/
    corecvs::Affine3DQ position;

    /* Configuration */
    bool cw = false;
    double maxForce = 9.8 / 3; /* Each motor is capable of just lifing itself */

    double propRadius  = 0.020; /**< in m **/
    double motorWidth  = 0.011; /**< in m **/
    double motorHeight = 0.004; /**< in m **/

    std::string name = "-";
    corecvs::RGBColor color = corecvs::RGBColor::Red();

    /* State */
    double pwm = 0.0; /**< 0..1 **/
    double phi = degToRad(32);

    void drawMyself(corecvs::Mesh3D &mesh)
    {
        if(mesh.hasColor) {
            mesh.setColor(color);
        }

        if (motorMesh != NULL) {
            mesh.add(*motorMesh);
        } else {
            mesh.addCylinder(Vector3dd::Zero(), motorWidth / 2, motorHeight, 10, 0);
        }
        mesh.mulTransform(Affine3DQ::RotationZ(phi));

        if (propMesh != NULL) {
            mesh.add(*propMesh);
        } else {
            mesh.addAOB(Vector3dd(-propRadius, -0.002 , motorHeight        ),
                        Vector3dd(+propRadius,  0.002 , motorHeight + 0.002));
        }
        mesh.popTransform();

    }

    Force getForce()
    {
        Vector3dd force(0, 0, maxForce * pwm);
        return Force(force, Vector3dd::Zero());
    }

    Vector3dd getM()
    {
        double k = 2 * pow(10, -6), b = 1 * pow(10, -7);
        double momentum = pwm / k * b * (cw ? 1: -1);
        //if(momentum!=0)
        //L_INFO<<"Momentum of "<<i<<" motor: "<<momentum;
        return Vector3dd(0.0, 0.0, momentum);
    }

    /* UI not owned. Need to be reworked. Could be reused for hardcoded solution */
    Mesh3D *motorMesh = NULL;
    Mesh3D *propMesh  = NULL;
};

class Sensor
{
public:
     corecvs::Affine3DQ position;
     AxisAlignedBox3d box;
     std::string name;
};

class Quad : public SimObject
{
public:
    std::vector<Motor> motors;
    std::vector<CameraModel> cameras;
    std::vector<Sensor> sensors;
    PID pitchPID{0.7, 0.35, 0.35};
    PID rollPID{0.7, 0.35, 0.35};
    PID yawPID{0.7, 0.35, 0.35};

    enum BetaflightMotors {
        BETAFLIGHT_MOTOR_1 = 0,
        BETAFLIGHT_MOTOR_2 = 1,
        BETAFLIGHT_MOTOR_3 = 2,
        BETAFLIGHT_MOTOR_4 = 3,

        MOTOR_BR = BETAFLIGHT_MOTOR_1,
        MOTOR_FR = BETAFLIGHT_MOTOR_2,
        MOTOR_BL = BETAFLIGHT_MOTOR_3,
        MOTOR_FL = BETAFLIGHT_MOTOR_4,

    };

    Vector3dd FromQuaternion(Quaternion &Q);

    Quad(double frameSize = 0.088);

    corecvs::Affine3DQ getMotorTransfrom(int num);

    corecvs::Affine3DQ  getTransform();

    void drawMyself(Mesh3D &mesh);
    void drawMyself(Mesh3DDecorated &mesh);

    void visualTick();

    void flightControllerTick(const CopterInputs &input);
    void physicsTick();

    /* UI */
    Mesh3D *bodyMesh = NULL;

    /* This definitely need to be in the other place */
    Mesh3DDecorated *worldMesh = NULL;

    ~Quad();
};



#endif // QUAD_H
