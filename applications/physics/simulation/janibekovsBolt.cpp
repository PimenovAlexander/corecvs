#include "janibekovsBolt.h"
#include <core/fileformats/meshLoader.h>
JanibekovsBolt::JanibekovsBolt(double arm, double mass)
{
    setSystemMass(mass);
    double massOfRotatingObjects = 1;
    double massOfTestingObject = 4;
    double massOfZeroObject = 0.0;
    double partsRadius = 0.001;
    Affine3DQ defaultPos = Affine3DQ(Vector3dd::Zero());

    partsOfSystem.push_back(PhysSphere(&defaultPos, &partsRadius, &massOfRotatingObjects));
    partsOfSystem.push_back(PhysSphere(&defaultPos, &partsRadius, &massOfRotatingObjects));
    partsOfSystem.push_back(PhysSphere(&defaultPos, &partsRadius, &massOfTestingObject));
    partsOfSystem.push_back(PhysSphere(&defaultPos, &partsRadius, &massOfZeroObject));

    partsOfSystem[0].color = RGBColor::Red();    /*Front right*/
    partsOfSystem[1].color = RGBColor::Red();  /*Back  left*/
    partsOfSystem[2].color = RGBColor::Green();    /*Front left*/
    partsOfSystem[3].color = RGBColor::Yellow();  /*Back  right*/

    partsOfSystem[0].setPos(Vector3dd( 0,  1, 0).normalised() * 3 * arm);
    partsOfSystem[1].setPos(Vector3dd( 0, -1, 0).normalised() * 3 * arm);
    partsOfSystem[2].setPos(Vector3dd( 1,  0, 0).normalised() * arm);
    partsOfSystem[3].setPos(Vector3dd(-1,  0, 0).normalised() * arm);

    MeshLoader loader;
    Mesh3DDecorated mesh;
    if (loader.load(&mesh, "models/OBJ.obj"))
    {
        /* mm -> m and shift to right position */
        worldMesh = new Mesh3DDecorated;
        mesh.transform(Matrix44::RotationZ(degToRad(90)) *
                       Matrix44::Shift(0, -20, -4) *
                       Matrix44::Scale(1/200.0) *
                       Matrix44::RotationX(degToRad(90)));

        worldMesh->add(mesh, true);
        worldMesh->recomputeMeanNormals();

        worldMesh->dumpInfo();
    }


    double massOfCentralSphere = mass - 2 * massOfRotatingObjects - massOfTestingObject;
    Affine3DQ posOfCentralSphere = Affine3DQ(Vector3dd(0,0,0).normalised());
    double radiusOfCentralSphere = arm / 100;
    centralSphere = PhysSphere(&posOfCentralSphere, &radiusOfCentralSphere, &massOfCentralSphere);

    objects.push_back(&centralSphere);
    for (size_t i = 0; i < partsOfSystem.size(); ++i)
    {
        objects.push_back(&partsOfSystem[i]);
    }
}


void JanibekovsBolt::drawMyself(Mesh3D &mesh)
{
        mesh.mulTransform(getTransform());
        mesh.switchColor();
/*
        centralSphere.drawMesh(mesh);
        for (int i = 0; i < partsOfSystem.size(); i++) {
            partsOfSystem[i].drawMesh(mesh);
        }
*/
        for (size_t i = 0; i < objects.size(); i++) {
            objects[i]->drawMesh(mesh);
        }
        mesh.popTransform();
}

Affine3DQ JanibekovsBolt::getTransform()
{
    return Affine3DQ(this->orientation, this->getPosCenter());
}

void JanibekovsBolt::drawMyself(Mesh3DDecorated &mesh)
{
    JanibekovsBolt::drawMyself((Mesh3D &)mesh);

    /* Scene should be drawed */

    //L_INFO << "Quad::drawMyself(Mesh3DDecorated &mesh): before\n";
    //mesh.dumpInfo();


    if (worldMesh != NULL)
    {
        mesh.add(*worldMesh);
    }


    //L_INFO << "Quad::drawMyself(Mesh3DDecorated &mesh): after\n";
    //mesh.dumpInfo();


}

void JanibekovsBolt::physicsTick(double deltaT)
{
    /*
    if(!testMode)
    {
        startTick();
        for (size_t i = 0; i < motors.size(); ++i) {
            motors[i].calcForce();
        }
    }
    */
    calcForce();
    calcMoment();

    /* TODO: Add air friction*/
    //addForce(Vector3dd(0.0, 0.0, -9.8 * getSystemMass()));
    tick(deltaT);

    /*TODO: Add real collistion comрutation */

    //cout << "Quad::physicsTick(): " << position << endl;
}




void JanibekovsBolt::tick(double deltaT)
{
    double radius = centralSphere.radius;
    double centerMass = centralSphere.mass;

    double massOfRotatingObjects = partsOfSystem[0].mass;
    double massOfTestingObject = partsOfSystem[2].mass;

    double armOfRotatingObjects = partsOfSystem[0].getPosVector().l2Metric();
    double armOfTestingObject = partsOfSystem[2].getPosVector().l2Metric();

    double inertialMomentX = 2.0 / 5.0 * centerMass * pow(radius, 2) + 2 * massOfRotatingObjects * pow(armOfRotatingObjects, 2);
    double inertialMomentY = 2.0 / 5.0 * centerMass * pow(radius, 2) + massOfTestingObject * pow(armOfTestingObject, 2);
    double inertialMomentZ = 2.0 / 5.0 * centerMass * pow(radius, 2) + 2 * massOfRotatingObjects * pow(armOfRotatingObjects, 2)
                                                                     + massOfTestingObject * pow(armOfTestingObject, 2);

/*
    inertialMomentX = 1;
    inertialMomentY = 0.20;
    inertialMomentZ = 5;
*/
    Matrix33 diagonalizedInertiaTensor = Matrix33(inertialMomentX, 0, 0,
                                                  0, inertialMomentY, 0,
                                                  0, 0, inertialMomentZ);

    using namespace std::chrono;
    time_t ms0 = duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
    ).count();
    if(ms0 % 200 == 0)
    {
        L_INFO<< "Diagonalized Tensor: " << diagonalizedInertiaTensor / inertialMomentX;
    }

    Matrix33 transposedOrient = orientation.toMatrix();
    transposedOrient.transpose();
    inertiaTensor = orientation.toMatrix() * diagonalizedInertiaTensor * transposedOrient;// orientation.toMatrix().transpose();

    velocity = Vector3dd(0.0, 0.0, 0.0);

    Vector3dd newPos = getPosCenter() + velocity * deltaT;
    if (newPos.z() < -0.1)
    {
        velocity = Vector3dd::Zero();
        setPosCenter(Vector3dd(newPos.x(), newPos.y(), -0.1));
    }
    else {
        setPosCenter(newPos);
    }
    velocity += (getForce() / getSystemMass()) * deltaT;

    /* We should carefully use inertiaTensor here. It seems like it changes with the frame of reference */
    //L_INFO << "Momentum: " << getMomentum();
    /**This works as wanted**/
    Vector3dd W = inertiaTensor.inv() * getMomentum();
    Quaternion angularAcceleration = Quaternion::pow(Quaternion::Rotation(W, W.l2Metric()), 0.000001);

    Quaternion q = orientation;
    //Probably bug here
    //angularVelocity = Quaternion(0.621634, 0, 0, -0.783308);
    orientation = Quaternion::pow(angularVelocity, deltaT * 1000) ^ orientation;

    //Just async output
    using namespace std::chrono;
    time_t ms = duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
    ).count();
    if(ms % 200 == 0)
    {
        //L_INFO<<"Delta orient: "<<orientation.getAngle()-q.getAngle();
    }

    //orientation.printAxisAndAngle();
    Quaternion temp = Quaternion::pow(angularAcceleration,deltaT);

    using namespace std::chrono;
    time_t ms1 = duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
    ).count();
    if(ms % 200 == 0)
    {
        //L_INFO << angularVelocity;
    }

    //Probably bug here
    angularVelocity = Quaternion::pow(angularAcceleration, deltaT * 1000) ^ angularVelocity;

    //L_INFO<<"Delta orient: "<<abs(orientation.getAngle()-q.getAngle());

}
