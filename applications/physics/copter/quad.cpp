#include "quad.h"

using namespace corecvs;

Quad::Quad(double frameSize)
{
    motors.resize(4);
    motors[0].name = "1"; motors[0].color = RGBColor::Red();
    motors[1].name = "2"; motors[1].color = RGBColor::Green();
    motors[2].name = "3"; motors[2].color = RGBColor::Red();
    motors[3].name = "4"; motors[3].color = RGBColor::Green();

    double arm = frameSize / 2;

    motors[0].position.shift = Vector3dd( 1,  1, 0).normalised() * arm;
    motors[1].position.shift = Vector3dd(-1, -1, 0).normalised() * arm;
    motors[2].position.shift = Vector3dd( 1, -1, 0).normalised() * arm;
    motors[3].position.shift = Vector3dd(-1,  1, 0).normalised() * arm;


    Affine3DQ camPos = Affine3DQ::RotationY(degToRad(90)) * Affine3DQ::RotationZ(degToRad(-90));
    cameras.resize(1);
    cameras[0].setLocation(Affine3DQ::Shift(0, 0, 0.02) * camPos);

    PinholeCameraIntrinsics *pinhole = new PinholeCameraIntrinsics(Vector2dd(640, 480), degToRad(60));
    cameras[0].intrinsics.reset(pinhole);

    sensors.resize(1);
    sensors[0].position = Affine3DQ::Shift(0, 0, 0.0);
    sensors[0].box = AxisAlignedBox3d(Vector3dd(-0.01, -0.01, -0.005), Vector3dd(0.01, 0.01, 0.005) );
}

Affine3DQ Quad::getMotorTransfrom(int num)
{
    return Affine3DQ(motors[num].position);
}

Affine3DQ Quad::getTransform()
{
    return Affine3DQ(orientation, position);
}


