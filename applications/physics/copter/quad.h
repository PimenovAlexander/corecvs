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
    bool cw;
    double maxForce;

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
        mesh.addCylinder(Vector3dd::Zero(), motorWidth / 2, motorHeight, 10, 0);
        mesh.mulTransform(Affine3DQ::RotationZ(phi));
        mesh.addAOB(Vector3dd(-propRadius, -0.002 , motorHeight        ),
                    Vector3dd(+propRadius,  0.002 , motorHeight + 0.002));
        mesh.popTransform();

    }

    Force getForce()
    {
        Vector3dd force(0, 0, maxForce * pwm);
        return Force(force, Vector3dd::Zero());
    }



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


    Quad(double frameSize = 0.088);

    corecvs::Affine3DQ getMotorTransfrom(int num);

    corecvs::Affine3DQ  getTransform();

    void drawMyself(corecvs::Mesh3D &mesh)
    {
        mesh.mulTransform(getTransform());

       for (size_t i = 0; i < motors.size(); i++)
       {
           Motor &motor = motors[i];
           mesh.mulTransform(corecvs::Matrix44(getMotorTransfrom(i)));
           motor.drawMyself(mesh);
           mesh.popTransform();

       }

       for (size_t i = 0; i < cameras.size(); i++)
       {
           CalibrationDrawHelpers helper;
           helper.setSolidCameras(true);
           helper.drawCamera(mesh, cameras[i], 0.01);
       }


       for (size_t i = 0; i < sensors.size(); i++)
       {
           mesh.mulTransform(sensors[i].position);
           mesh.addAOB(sensors[i].box);
           mesh.popTransform();
       }
       mesh.popTransform();

    }

    void visualTick()
    {
        for (size_t i = 0; i < motors.size(); i++)
        {
            motors[i].phi += (motors[i].cw ? motors[i].pwm : -motors[i].pwm) / 50.0;
        }
    }

    void flightControllerTick(const CopterInputs &input)
    {
        /* Mixer */
        double throttle = input.axis[CopterInputs::CHANNEL_THROTTLE];
        double pitch    = input.axis[CopterInputs::CHANNEL_PITCH];
        double yaw      = input.axis[CopterInputs::CHANNEL_YAW];
        double roll     = input.axis[CopterInputs::CHANNEL_ROLL];

        motors[0].pwm = - pitch +  roll + yaw + throttle;
        motors[1].pwm =   pitch + -roll + yaw + throttle;
        motors[2].pwm = - pitch + -roll - yaw + throttle;
        motors[3].pwm =   pitch +  roll - yaw + throttle;
    }




};

#endif // QUAD_H
