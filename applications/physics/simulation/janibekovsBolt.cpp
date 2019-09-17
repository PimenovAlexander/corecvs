#include "janibekovsBolt.h"

JanibekovsBolt::JanibekovsBolt(double arm, double mass)
{
    setSystemMass(mass);
    double massOfRotatingObjects = 0.05;
    double massOfTestingObject = 0.05;
    double partsRadius = 0.01;
    Affine3DQ defaultPos = Affine3DQ(Vector3dd::Zero());

    partsOfSystem.push_back(PhysSphere(&defaultPos, &partsRadius, &massOfRotatingObjects));
    partsOfSystem.push_back(PhysSphere(&defaultPos, &partsRadius, &massOfTestingObject));
    partsOfSystem.push_back(PhysSphere(&defaultPos, &partsRadius, &massOfRotatingObjects));
    partsOfSystem.push_back(PhysSphere(&defaultPos, &partsRadius, 0));

    partsOfSystem[0].color = RGBColor::Red();    /*Front right*/
    partsOfSystem[1].color = RGBColor::Green();  /*Back  left*/
    partsOfSystem[2].color = RGBColor::Red();    /*Front left*/
    partsOfSystem[3].color = RGBColor::Yellow();  /*Back  right*/

    partsOfSystem[0].setPos(Vector3dd( 1,  1, 0).normalised() * arm);
    partsOfSystem[1].setPos(Vector3dd(-1, -1, 0).normalised() * arm);
    partsOfSystem[2].setPos(Vector3dd( 1, -1, 0).normalised() * arm);
    partsOfSystem[3].setPos(Vector3dd(-1,  1, 0).normalised() * arm);

    double massOfCentralSphere = mass - 2 * massOfRotatingObjects - massOfTestingObject;
    Affine3DQ posOfCentralSphere = Affine3DQ(Vector3dd(0,0,0).normalised());
    double radiusOfCentralSphere = arm;
    centralSphere = PhysSphere(&posOfCentralSphere, &radiusOfCentralSphere, &massOfCentralSphere);

    objects.push_back(&centralSphere);
    for (size_t i = 0; i < partsOfSystem.size(); ++i)
    {
        objects.push_back(&partsOfSystem[i]);
    }
}
