#include <chrono>

#include "dzhanibekovBolt.h"
#include <core/fileformats/meshLoader.h>

using namespace std::chrono;

DzhanibekovBolt::DzhanibekovBolt(double arm, double mass)
{
    setSystemMass(mass);
    double massOfRotatingObjects = 6;
    double massOfTestingObject = 1;
    double massOfZeroObject = 0.0;
    double partsRadius = 0.01;
    Affine3DQ defaultPos = Affine3DQ(Vector3dd::Zero());

    systemElements.push_back(new PhysicsSphere(defaultPos, partsRadius, massOfRotatingObjects));
    systemElements.push_back(new PhysicsSphere(defaultPos, partsRadius, massOfRotatingObjects));
    systemElements.push_back(new PhysicsSphere(defaultPos, partsRadius, massOfTestingObject));
    systemElements.push_back(new PhysicsSphere(defaultPos, partsRadius, massOfZeroObject));

    systemElements[0]->color = RGBColor::Red();    /*Front right*/
    systemElements[1]->color = RGBColor::Red();    /*Back  left*/
    systemElements[2]->color = RGBColor::Green();  /*Front left*/
    systemElements[3]->color = RGBColor::Yellow(); /*Back  right*/

    systemElements[0]->setPos(Vector3dd( 0,  1, 0).normalised() * 3 * arm);
    systemElements[1]->setPos(Vector3dd( 0, -1, 0).normalised() * 3 * arm);
    systemElements[2]->setPos(Vector3dd( 1,  0, 0).normalised() * arm);
    systemElements[3]->setPos(Vector3dd(-1,  0, 0).normalised() * arm);

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
    PhysicsSphere *centralSphere = new PhysicsSphere(posOfCentralSphere, radiusOfCentralSphere, massOfCentralSphere);

    objects.push_back(centralSphere);
    for (size_t i = 0; i < systemElements.size(); ++i)
    {       
        objects.push_back(systemElements[i]);
    }
}


void DzhanibekovBolt::drawMyself(Mesh3D &mesh)
{
        mesh.mulTransform(getTransform());
        mesh.switchColor();

        centralSphere->drawMesh(mesh);
        for (int i = 0; i < systemElements.size(); i++) {
            systemElements[i]->drawMesh(mesh);
        }

        Circle3d circle;
        circle.c = Vector3dd(0,0,0);
        circle.r = 0.3;
        circle.normal = Vector3dd(1,0,0);
        mesh.addCircle(circle);
        mesh.currentColor = RGBColor::Violet();
        mesh.addLine(objects[3]->position(), objects[4]->position());
        mesh.addLine(objects[1]->position(), objects[2]->position());
        mesh.popTransform();
        mesh.currentColor = RGBColor::Cyan();
        drawForces(mesh);
}

void DzhanibekovBolt::drawForces(Mesh3D &mesh)
{
    for (size_t i = 0; i < systemElements.size(); i++)
    {
        Affine3DQ motorToWorld = getTransform() * systemElements[i]->affine();
        Vector3dd force = motorToWorld.rotor * systemElements[i]->force();
        Vector3dd motorPosition = motorToWorld.shift;
        Vector3dd startDot = motorPosition;
        Vector3dd endDot = motorPosition - force * 20.0;
        mesh.addLine(startDot, endDot);

        //L_INFO << "force: " << force << ", position " << motorPosition;
        //L_INFO << "Drew force line from: " << startDot << " , to: " << endDot << " ; Force value: " << force;
    }
}

DzhanibekovBolt::~DzhanibekovBolt()
{
    delete_safe(centralSphere);
    for (PhysicsSphere *element : systemElements)
    {
        delete_safe(element);
    }
    systemElements.clear();

}

Affine3DQ DzhanibekovBolt::getTransform()
{
    return this->x;
}

void DzhanibekovBolt::drawMyself(Mesh3DDecorated &mesh)
{
    DzhanibekovBolt::drawMyself((Mesh3D &)mesh);

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

void DzhanibekovBolt::physicsTick(double deltaT)
{
    calcForce();
    calcMoment();

    /* TODO: Add air friction*/
    //addForce(Vector3dd(0.0, 0.0, -9.8 * getSystemMass()));
    tick(deltaT);

    /*TODO: Add real collistion comрutation */

    //cout << "Quad::physicsTick(): " << position << endl;
}




void DzhanibekovBolt::tick(double deltaT)
{
    /** Sources:
     * https://habr.com/ru/post/264381/
     * https://www.euclideanspace.com/physics/kinematics/angularvelocity/
     * https://fgiesen.wordpress.com/2012/08/24/quaternion-differentiation/
     * Physically Based Modeling Rigid Body Simulation. David Baraff
     * https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-07-dynamics-fall-2009/lecture-notes/MIT16_07F09_Lec26.pdf
    **/

    double radius = centralSphere->radius;
    double centerMass = centralSphere->mass;

    double massOfRotatingObjects = systemElements[0]->mass;
    double massOfTestingObject   = systemElements[2]->mass;

    double armOfRotatingObjects = systemElements[0]->position().l2Metric();
    double armOfTestingObject   = systemElements[2]->position().l2Metric();

    double inertiaMomentX = 2.0 / 5.0 * centerMass * pow(radius, 2) + 2 * massOfRotatingObjects * pow(armOfRotatingObjects, 2);
    double inertiaMomentY = 2.0 / 5.0 * centerMass * pow(radius, 2) + massOfTestingObject * pow(armOfTestingObject, 2);
    double inertiaMomentZ = 2.0 / 5.0 * centerMass * pow(radius, 2) + 2 * massOfRotatingObjects * pow(armOfRotatingObjects, 2)
                                                                     + massOfTestingObject * pow(armOfTestingObject, 2);

    //Matrix33 diagonalizedInertiaTensor = Matrix33::FromDiagonal(1.0, 1.5, 0.75);
    //Matrix33 diagonalizedInertiaTensor = Matrix33::FromDiagonal(0.001, 0.002, 0.0001);
    Matrix33 diagonalizedInertiaTensor = Matrix33::FromDiagonal(inertiaMomentX, inertiaMomentY, inertiaMomentZ);

    Matrix33 transposedOrient = x.rotor.toMatrix().transposed();
    inertiaTensor = diagonalizedInertiaTensor;

    /*
    if(testMode)
    {
        inertiaTensor = orientation.toMatrix() * diagonalizedInertiaTensor * transposedOrient;// orientation.toMatrix().transpose();
    }
    else
    {
        inertiaTensor = diagonalizedInertiaTensor;
    }
    */

    time_t ms0 = duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
    ).count();
    if(ms0 % 200 == 0)
    {
        //Matrix33 invAngVel = angularVelocity.toMatrix();
        //L_INFO << invAngVel;
        //L_INFO << orientation;
        //L_INFO<< "Diagonalized Tensor: " << diagonalizedInertiaTensor / inertialMomentX;
        //L_INFO << "Inertia tensor: " << inertiaTensor/inertialMomentX;
    }

    /** Kinematics **/
    v = Vector3dd(0.0, 0.0, 0.0);

    Vector3dd newPos = getPosCenter() + v * deltaT;
    /** Floor simulation **/
    if (newPos.z() < -0.1)
    {
        v = Vector3dd::Zero();
        setPosCenter(Vector3dd(newPos.x(), newPos.y(), -0.1));
    }
    else {
        setPosCenter(newPos);
    }
    v += (force() / getSystemMass()) * deltaT;

    /* We should carefully use inertiaTensor here. It seems like it changes with the frame of reference */
    /** Dynamics **/
    Vector3dd w_modified = w * deltaT;
    Quaternion dq = Quaternion::Rotation(w_modified, w_modified.l2Metric());
    x.rotor = x.rotor ^ dq;

    Matrix33 omega = Matrix33::CrossProductLeft(w);
    Vector3dd dw = inertiaTensor.inv() * (momentum() - (omega * inertiaTensor * w_modified));
    w += dw;// * deltaT;

    /** Need more info about why this is needed **/
    x.rotor.normalise();

    /** Understood what this should do, but have no clue how to apply this to object
     * without initial angular veclocity (like DzhanibekovVideo test) **/
    //double mw = w.l2Metric();
    //angularVelocity *= mw / angularVelocity.l2Metric();

    /** Old physics equations, could be useful **/
    //Vector3dd W = inertiaTensor.inv() * getMomentum();
    //angularAcceleration = Quaternion::pow(Quaternion::Rotation(W, W.l2Metric()), 0.000001);
    //orientation = Quaternion::pow(angularVelocity, deltaT * 1000) ^ orientation;
    //angularVelocity = Quaternion::pow(angularAcceleration, deltaT * 1000) ^ angularVelocity;

    /** Just output **/
    if(ms0 % 200 == 0)
    {
        //L_INFO<<"Delta orient: "<<orientation.getAngle()-q.getAngle();
        //L_INFO << angularAcceleration;
        //L_INFO << orientation;
    }
}
