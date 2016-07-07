#include "gentryState.h"
#include "mesh3d.h"
#include "calibrationHelpers.h"


long double operator ""_deg(long double in)
{
    return degToRad(in);
}

using namespace corecvs;

int main(int argc, char *argv[])
{
    GentryState state;
    Vector2dd resolution(640,480);

    double t = 56.0_deg;


    state.camera.intrinsics = PinholeCameraIntrinsics(resolution,  degToRad(60));
    state.camera.setLocation(
                Affine3DQ::Shift(0,0,10)
                );
    state.laserPlane = Plane3d::FormNormalAndPoint(
                Vector3dd(0,0,1),
                Vector3dd::Zero()
                );


    Mesh3D debug;
    CalibrationHelpers drawer;
    drawer.drawCamera(debug, state.camera, 1.0);

    Circle3d pd;
    pd.c = Vector3dd::Zero();
    pd.normal = state.laserPlane.normal();
    pd.r = 100;
    debug.addCircle(pd);

    debug.dumpPLY("debug.ply");

}

