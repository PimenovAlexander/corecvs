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

    Vector3dd getM()
    {
        return Vector3dd(0.0, 0.0, pwm * cw);
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

    void drawMyself(corecvs::Mesh3D &mesh);

    void visualTick();

    void flightControllerTick(const CopterInputs &input);

    void physicsTick();

};



#endif // QUAD_H
