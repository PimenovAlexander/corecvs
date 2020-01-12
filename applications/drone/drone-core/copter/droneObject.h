#ifndef DRONEOBJECT_H
#define DRONEOBJECT_H

#include <copterInputs.h>
#include <vector>
#include "motor.h"

#include <core/cameracalibration/cameraModel.h>
#include "core/cameracalibration/calibrationDrawHelpers.h"
#include "core/geometry/mesh3d.h"
#include "core/math/affine.h"
#include "core/math/vector/vector3d.h"

#include "physicsCompoundObject.h"
#include "pid.h"

class Sensor
{
public:
     corecvs::Affine3DQ position;
     AxisAlignedBox3d box;
     std::string name;
};

class DroneObject : public PhysicsMainObject
{
public:
    std::vector<CameraModel> cameras;
    std::vector<Sensor> sensors;

    /* Not owned */
    std::vector<Motor *> motors;

    /* Not owned */
    PhysicsSphere *centralSphere;
    bool testMode = false;

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

    DroneObject(double frameSize = 0.088, double mass = 0.12);

    virtual void tick(double deltaT) override;

    Vector3dd fromQuaternion(Quaternion &Q);
    corecvs::Affine3DQ  getTransform();
    void drawMyself(Mesh3D &mesh);
    void drawMyself(Mesh3DDecorated &mesh);
    void flightControllerTick(const CopterInputs &input);
    void visualTick();
    void physicsTick(double deltaT);
    /* UI */
    Mesh3D *bodyMesh = NULL;

    /* This definitely need to be in the other place */
    Mesh3DDecorated *worldMesh = NULL;

    ~DroneObject() override;
private:
    void drawForces(Mesh3D &mesh);
    void drawSensors(Mesh3D &mesh);
    void drawCameras(Mesh3D &mesh);
    void drawMotors(Mesh3D &mesh);
    void drawBody(Mesh3D &mesh);
    //Vector3dd getForceTransformed(const Affine3DQ &T, Motor &obj);
};

#endif // DRONEOBJECT_H
