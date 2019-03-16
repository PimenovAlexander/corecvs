#include "quad.h"

using namespace corecvs;

Quad::Quad(double frameSize)
{
    position = Vector3dd::Zero();
    mass = 0.2; /* 200g copter */

    motors.resize(4);
    motors[BETAFLIGHT_MOTOR_2].name = "FR"; motors[BETAFLIGHT_MOTOR_2].color = RGBColor::Red();    /*Front right*/
    motors[BETAFLIGHT_MOTOR_3].name = "BL"; motors[BETAFLIGHT_MOTOR_3].color = RGBColor::Green();  /*Back  left*/
    motors[BETAFLIGHT_MOTOR_4].name = "FL"; motors[BETAFLIGHT_MOTOR_4].color = RGBColor::Red();    /*Front left*/
    motors[BETAFLIGHT_MOTOR_1].name = "BR"; motors[BETAFLIGHT_MOTOR_1].color = RGBColor::Green();  /*Back  right*/

    double arm = frameSize / 2;

    motors[BETAFLIGHT_MOTOR_2].position.shift = Vector3dd( 1,  1, 0).normalised() * arm; motors[BETAFLIGHT_MOTOR_2].cw = true;
    motors[BETAFLIGHT_MOTOR_3].position.shift = Vector3dd(-1, -1, 0).normalised() * arm; motors[BETAFLIGHT_MOTOR_3].cw = true;
    motors[BETAFLIGHT_MOTOR_4].position.shift = Vector3dd( 1, -1, 0).normalised() * arm; motors[BETAFLIGHT_MOTOR_4].cw = false;
    motors[BETAFLIGHT_MOTOR_1].position.shift = Vector3dd(-1,  1, 0).normalised() * arm; motors[BETAFLIGHT_MOTOR_1].cw = false;


    Affine3DQ camPos = Affine3DQ::RotationY(degToRad(90)) * Affine3DQ::RotationZ(degToRad(-90));
    cameras.resize(1);
    cameras[0].setLocation(Affine3DQ::Shift(0, 0, 0.02) * camPos);

    PinholeCameraIntrinsics *pinhole = new PinholeCameraIntrinsics(Vector2dd(640, 480), degToRad(60));
    cameras[0].intrinsics.reset(pinhole);

    sensors.resize(1);
    sensors[0].position = Affine3DQ::Shift(0, 0, 0.0);
    sensors[0].box = AxisAlignedBox3d(Vector3dd(-0.01, -0.01, -0.005), Vector3dd(0.01, 0.01, 0.005) );



    /* Load ui*/
    //Mesh3D *body = Mes
}

Affine3DQ Quad::getMotorTransfrom(int num)
{
    return Affine3DQ(motors[num].position);
}

Affine3DQ Quad::getTransform()
{
    return Affine3DQ(orientation, position);
}

void Quad::drawMyself(Mesh3D &mesh)
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


    /* Draw Forces in world coordinates */

    for (size_t i = 0; i < motors.size(); i++)
    {
        Affine3DQ motorToWorld = getTransform() * getMotorTransfrom(i);
        Force f = motors[i].getForce().transformed(motorToWorld);
        mesh.addLine(f.position, f.position + f.force * 1.0);
    }

}

void Quad::flightControllerTick(const CopterInputs &input)
{
    /* Mixer */
    double throttle = input.axis[CopterInputs::CHANNEL_THROTTLE];
    double pitch    = input.axis[CopterInputs::CHANNEL_PITCH];
    double yaw      = input.axis[CopterInputs::CHANNEL_YAW];
    double roll     = input.axis[CopterInputs::CHANNEL_ROLL];

    motors[BETAFLIGHT_MOTOR_2].pwm = - pitch +  roll + yaw + throttle;
    motors[BETAFLIGHT_MOTOR_3].pwm =   pitch + -roll + yaw + throttle;
    motors[BETAFLIGHT_MOTOR_4].pwm = - pitch + -roll - yaw + throttle;
    motors[BETAFLIGHT_MOTOR_1].pwm =   pitch +  roll - yaw + throttle;

    for (size_t i = 0; i < motors.size(); i++)
    {
        motors[i].pwm = motors[i].pwm / 100.0;

        if (motors[i].pwm < 0.0) motors[i].pwm = 0.0;
        if (motors[i].pwm > 1.0) motors[i].pwm = 1.0;
    }

}

void Quad::physicsTick()
{
    startTick();
    for (size_t i = 0; i < motors.size(); i++)
    {
        Affine3DQ motorToWorld = getTransform() * motors[i].position;

        addForce (motors[i].getForce().transformed(motorToWorld));
        /* TODO: Transform moment */
        addMoment(motors[i].getM());
    }
    /* TODO: Add air friction*/
    addForce(Force(Vector3dd(0, 0, -9.8)));
    tick(0.1);

    /*TODO: Add real collistion comutation */
    if (position.z() < -1.0)
    {
        velocity = Vector3dd::Zero();
        position.z() = -1.0;
    }

    //cout << "Quad::physicsTick(): " << position << endl;

}



void Quad::visualTick()
{
    for (size_t i = 0; i < motors.size(); i++)
    {
        motors[i].phi += (motors[i].cw ? motors[i].pwm : -motors[i].pwm) / 50.0;
    }
}
