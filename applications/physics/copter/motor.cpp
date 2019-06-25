#include "motor.h"

Motor::Motor() : PhysSphere()
{
    cw = false;
    L_INFO << "Created default Motor";
}


Motor::Motor(Affine3DQ *pos, double *propellerRadius, double *mass)
    : PhysSphere (pos, propellerRadius, mass)
{
    cw = false;
    L_INFO << "Created Motor with pos: " << pos << " , propeller radius: "
           << propellerRadius << " , motor mass: " << mass;
}


void Motor::calcMoment()
{
    addMoment(getForce() * getPosVector());
    Vector3dd v = calcMotorMoment();
    L_INFO << "Motor.calcMoment() was called. Moment of current motor is: " << v;
}

void Motor::drawMesh(corecvs::Mesh3D &mesh)
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
        mesh.addAOB(Vector3dd(-radius, -0.002 , motorHeight        ),
                    Vector3dd(+radius,  0.002 , motorHeight + 0.002));
    }
    mesh.popTransform();

    SYNC_PRINT(("Successfully drew motor mesh"));
}

void Motor::calcForce()
{
    Vector3dd _force = Vector3dd(0, 0, maxForce * pwm);
    addForce(_force);
    L_INFO << "Motor.calcForce() called. Added force: " << _force;
}

Vector3dd Motor::getForceTransformed(const Affine3DQ &T)
{
    return T.rotor * getForce();
}

Vector3dd Motor::calcMotorMoment()
{
    double k = 2 * pow(10, -6), b = 1 * pow(10, -7);
    double momentum = pwm / k * b * (cw ? 1: -1);
    //if(momentum!=0)
    //L_INFO<<"Momentum of "<<i<<" motor: "<<momentum;
    addMoment(Vector3dd(0.0, 0.0, momentum));
    return Vector3dd(0.0, 0.0, momentum);
}
