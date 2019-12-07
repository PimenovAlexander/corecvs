#include "droneObject.h"
#include "core/utils/log.h"

#include <core/fileformats/meshLoader.h>

using namespace corecvs;
using namespace std::chrono;
DroneObject::DroneObject(double frameSize, double mass) : PhysMainObject()
{
    setSystemMass(mass);
    double propellerRadius = 0.020; /**< propeller radius in m **/
    double motorMass = 0.01; /**< in kg **/
    Affine3DQ defaultPos = Affine3DQ(Vector3dd::Zero());

    motors.resize(4, Motor(&defaultPos, &propellerRadius, &motorMass));
    motors[MOTOR_FR].name = "FR"; motors[MOTOR_FR].color = RGBColor::Red();    /*Front right*/
    motors[MOTOR_BL].name = "BL"; motors[MOTOR_BL].color = RGBColor::Green();  /*Back  left*/
    motors[MOTOR_FL].name = "FL"; motors[MOTOR_FL].color = RGBColor::Red();    /*Front left*/
    motors[MOTOR_BR].name = "BR"; motors[MOTOR_BR].color = RGBColor::Green();  /*Back  right*/

    double arm = frameSize / 2;

    motors[MOTOR_FR].setPos(Vector3dd( 1,  1, 0).normalised() * arm); motors[MOTOR_FR].cw = true;
    motors[MOTOR_BL].setPos(Vector3dd(-1, -1, 0).normalised() * arm); motors[MOTOR_BL].cw = true;
    motors[MOTOR_FL].setPos(Vector3dd( 1, -1, 0).normalised() * arm); motors[MOTOR_FL].cw = false;
    motors[MOTOR_BR].setPos(Vector3dd(-1,  1, 0).normalised() * arm); motors[MOTOR_BR].cw = false;

    Affine3DQ camPos = Affine3DQ::RotationY(degToRad(90)) * Affine3DQ::RotationZ(degToRad(-90));
    cameras.resize(1);
    cameras[0].setLocation(Affine3DQ::Shift(0, 0, 0.02) * camPos);

    PinholeCameraIntrinsics *pinhole = new PinholeCameraIntrinsics(Vector2dd(640, 480), degToRad(60));
    cameras[0].intrinsics.reset(pinhole);

    sensors.resize(1);
    sensors[0].position = Affine3DQ::Shift(0, 0, 0.0);
    sensors[0].box = AxisAlignedBox3d(Vector3dd(-0.01, -0.01, -0.005), Vector3dd(0.01, 0.01, 0.005) );

    /* Load ui*/
    MeshLoader loader;
    {
        Mesh3D mesh;
        if (loader.load(&mesh, "models/75mm_whoop_frame_v1.stl"))
        {
            mesh.transform(Matrix44::Scale(88.0/75.0) *
                           Matrix44::RotationX(degToRad(-90)) *
                           Matrix44::Scale(1/1000.0) *
                           Matrix44::Shift(-23, 0, -23));
            bodyMesh = new Mesh3D;
            bodyMesh->add(mesh);
        }
    }

    {
        Mesh3D mesh;
        if (loader.load(&mesh, "models/CW_Tri_Prop.stl"))
        {
            /* mm -> m and shift to right position */
            mesh.transform(Matrix44::Scale(2.0/5.0) * Matrix44::Scale(1/1000.0) * Matrix44::Shift(-236, -207, 0));

            motors[MOTOR_FR].propMesh = new Mesh3D;
            motors[MOTOR_FR].propMesh->add(mesh);

            motors[MOTOR_BL].propMesh = new Mesh3D;
            motors[MOTOR_BL].propMesh->add(mesh);
        }
    }

    {
        Mesh3D mesh;
        if (loader.load(&mesh, "models/CCW_Tri_Prop.stl"))
        {
            /* mm -> m and shift to right position */
            mesh.transform(Matrix44::Scale(2.0/5.0) * Matrix44::Scale(1/1000.0) * Matrix44::Shift(-372, -208, 0));

            motors[MOTOR_BR].propMesh = new Mesh3D;
            motors[MOTOR_BR].propMesh->add(mesh);

            motors[MOTOR_FL].propMesh = new Mesh3D;
            motors[MOTOR_FL].propMesh->add(mesh);
        }
    }

    {
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
    }

    double massOfCentralSphere = mass - 4 * motors[0].mass;
    Affine3DQ posOfCentralSphere = Affine3DQ(Vector3dd(0,0,0).normalised());
    double radiusOfCentralSphere = arm / 2;
    centralSphere = PhysSphere(&posOfCentralSphere, &radiusOfCentralSphere, &massOfCentralSphere);
    //motors.insert(motors.end(), motors.begin(), motors.end());

    //L_INFO<< "centralSphere pos: " << centralSphere.getPosVector()
    //      << " , mass: " << centralSphere.mass << " , radius: " << centralSphere.radius;
    objects.push_back(&centralSphere);
    //objects.push_back(&motors[0]);

    for (size_t i = 0; i < motors.size(); ++i)
    {
        objects.push_back(&motors[i]);
    }
}


void DroneObject::drawMyself(Mesh3D &mesh)
{
    visualTick();
    mesh.mulTransform(getTransform());
    mesh.switchColor();

    drawBody(mesh);
    drawMotors(mesh);
    drawCameras(mesh);
    drawSensors(mesh);
    mesh.popTransform();
    drawForces(mesh);
}

void DroneObject::drawBody(Mesh3D &mesh)
{
    if (bodyMesh != NULL)
    {
        mesh.setColor(RGBColor::White());
        mesh.add(*bodyMesh);
        mesh.popTransform();
    }
}

void DroneObject::drawMotors(Mesh3D &mesh)
{
    for (size_t i = 0; i < motors.size(); i++)
    {
        Motor motor = motors[i];
        mesh.mulTransform(corecvs::Matrix44(motors[i].getPosAffine()));
        motor.drawMesh(mesh);
        //L_INFO << "motor->drawMesh(mesh) passed";
        /** Very bad kludge, can be used only as a last resort **/
        /*
        if (Motor* motor = dynamic_cast<Motor*>(this))
                motor->drawMyself(mesh);
        else
            L_INFO << "dynamic_cast at droneObject.drawMyself() unexpectedly failed";
        */
        mesh.popTransform();
    }
}

void DroneObject::drawCameras(Mesh3D &mesh)
{
    for (size_t i = 0; i < cameras.size(); i++)
    {
        CalibrationDrawHelpers helper;
        helper.setSolidCameras(true);
        helper.drawCamera(mesh, cameras[i], 0.01);
    }

}

void DroneObject::drawSensors(Mesh3D &mesh)
{
    for (size_t i = 0; i < sensors.size(); i++)
    {
        mesh.mulTransform(sensors[i].position);
        mesh.addAOB(sensors[i].box);
        mesh.popTransform();
    }
}

void DroneObject::drawForces(Mesh3D &mesh)
{
    for (size_t i = 0; i < motors.size(); i++)
    {
        Affine3DQ motorToWorld = getTransform() * motors[i].getPosAffine();
        Vector3dd force = motorToWorld.rotor * motors[i].getForce();
        Vector3dd motorPosition = motorToWorld.shift;
        Vector3dd startDot = motorPosition;
        Vector3dd endDot = motorPosition + force * 1.0;
        mesh.addLine(startDot, endDot);

        //L_INFO << "force: " << force << ", position " << motorPosition;
        //L_INFO << "Drew force line from: " << startDot << " , to: " << endDot << " ; Force value: " << force;
    }
}



Affine3DQ DroneObject::getTransform()
{
    return Affine3DQ(this->orientation, this->getPosCenter());
}

void DroneObject::drawMyself(Mesh3DDecorated &mesh)
{
    DroneObject::drawMyself((Mesh3D &)mesh);
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

Vector3dd DroneObject::fromQuaternion(Quaternion &Q)
{
    double yaw   = asin  (2.0 * (Q.t() * Q.y() - Q.z() * Q.x()));
    double pitch = atan2 (2.0 * (Q.t() * Q.x() + Q.y() * Q.z()),1.0 - 2.0 * (Q.x() * Q.x() + Q.y() * Q.y()));
    double roll  = atan2 (2.0 * (Q.t() * Q.z() + Q.x() * Q.y()),1.0 - 2.0 * (Q.y() * Q.y() + Q.z() * Q.z()));
    return Vector3dd(pitch, roll, yaw);
}
/*
PID::PID(double p, double i, double d)
{
    P=p;
    I=i;
    D=d;
}
*/
void DroneObject::flightControllerTick(const CopterInputs &input)
{
    /* Mixer */
    double throttle       = (input.axis[CopterInputs::CHANNEL_THROTTLE] - 1500) / 12;
    double wantedPitch    = (input.axis[CopterInputs::CHANNEL_PITCH]    - 1500) / 12;
    double wantedYaw      = (input.axis[CopterInputs::CHANNEL_YAW]      - 1500) / 12;
    double wantedRoll     = (input.axis[CopterInputs::CHANNEL_ROLL]     - 1500) / 12;

    /** Get current Pitch, Roll, Yaw of drone at this moment **/
    //Quaternion q = angularVelocity;
    Vector3dd currentPRY = fromQuaternion(orientation);

    for (int i = 0; i < 3; i++)
    {
        if (currentPRY[i]<0.001)
        {
            currentPRY[i]=0;
        }
    }

    double forceP, forceR, forceY;
    //--------------------------------------NEED FIX TIME!!!!!
    double deltaT=0.1;
    /** Get current between PRY now and wanted **/
/*
 * Vector3dd currentError(wantedPitch - currentPRY.x(),
                           wantedRoll  - currentPRY.y(),
                           wantedRoll  - currentPRY.z());

    pitchPID.sumOfError+=currentError.x();
    rollPID.sumOfError+=currentError.y();
    yawPID.sumOfError+=currentError.z();

    forceP = pitchPID.P * currentError.x() +
             pitchPID.I * deltaT * pitchPID.sumOfError +
             pitchPID.D * (currentError.x() - pitchPID.prevError) / deltaT;

    forceR = rollPID.P * currentError.y() +
             rollPID.I * deltaT * rollPID.sumOfError +
             rollPID.D * (currentError.y() - rollPID.prevError) / deltaT;

    forceY = yawPID.P * currentError.z() +
             yawPID.I * deltaT * yawPID.sumOfError +
             yawPID.D * (currentError.z() - yawPID.prevError) / deltaT;
    //L_INFO << "pitch previous error: " << pitchPID.prevError;
    pitchPID.prevError = currentError.x();
    rollPID.prevError  = currentError.y();
    yawPID.prevError   = currentError.z();
*/

    forceP = pitchPID.calculate(deltaT, wantedPitch, currentPRY.x());
    forceR = rollPID.calculate(deltaT , wantedRoll , currentPRY.y());
    forceY = yawPID.calculate(deltaT  , wantedYaw  , currentPRY.z());
/*
    motors[MOTOR_FR].pwm = - wantedPitch +  wantedRoll + wantedYaw + throttle;
    motors[MOTOR_BL].pwm =   wantedPitch + -wantedRoll + wantedYaw + throttle;
    motors[MOTOR_FL].pwm = - wantedPitch + -wantedRoll - wantedYaw + throttle;
    motors[MOTOR_BR].pwm =   wantedPitch +  wantedRoll - wantedYaw + throttle;
*/

    motors[MOTOR_FR].pwm = - forceP + forceR + forceY + throttle;
    motors[MOTOR_BL].pwm =   forceP - forceR + forceY + throttle;
    motors[MOTOR_FL].pwm = - forceP - forceR - forceY + throttle;
    motors[MOTOR_BR].pwm =   forceP + forceR - forceY + throttle;

    //L_INFO<<"PitchPID P: "<<pitchPID.P<<"; I: "<<pitchPID.I<<"; D: "<<pitchPID.D
    //     <<"; Current Error: "<<currentError.x()<<"; Prev Error: "<<pitchPID.prevError<<"; Sum Error: "<<pitchPID.sumOfError;
    //L_INFO<<"Forces : "<<forceP<<" ; "<<forceR<<" ; "<<forceY<<" ; "<<throttle;
    //L_INFO<<"Angular acc: "<<angularAcc;
    //L_INFO<<"W: "<<vec_W;
    //L_INFO<<"Angle Vel: "<<angularVelocity;
    //L_INFO<<"Orientation: "<<orientation;
    //L_INFO<<"Wanted PRY: "<<wantedPitch<<" ; "<<wantedRoll<<" ; "<<wantedYaw;
    //L_INFO<<"Current PRY: "<<currentPRY;
    //L_INFO<<"Current Error: "<<currentError;
    //L_INFO<<"Motor Values: "<<motors[0].pwm<<" ; "<<motors[1].pwm<<" ; "<<motors[2].pwm<<" ; "<<motors[3].pwm;

    for (size_t i = 0; i < motors.size(); i++)
    {
        motors[i].pwm = motors[i].pwm / 100.0;
        if (motors[i].pwm < 0.0) motors[i].pwm = 0.0;
        if (motors[i].pwm > 1.0) motors[i].pwm = 1.0;
    }
    //L_INFO<<"Motor True Values: "<<motors[0].pwm<<" ; "<<motors[1].pwm<<" ; "<<motors[2].pwm<<" ; "<<motors[3].pwm;
}

void DroneObject::physicsTick(double deltaT)
{
    if(!testMode)
    {
        startTick();
        for (size_t i = 0; i < motors.size(); ++i) {
            motors[i].calcForce();
        }
    }
    calcForce();
    calcMoment();

    /* TODO: Add air friction*/
    addForce(Vector3dd(0.0, 0.0, -9.8 * getSystemMass()));
    tick(deltaT);

    /*TODO: Add real collistion comрutation */

    //cout << "Quad::physicsTick(): " << position << endl;
}

void DroneObject::tick(double deltaT)
{
    /** Sources:
     * https://habr.com/ru/post/264381/
     * https://www.euclideanspace.com/physics/kinematics/angularvelocity/
     * https://fgiesen.wordpress.com/2012/08/24/quaternion-differentiation/
     * Physically Based Modeling Rigid Body Simulation. David Baraff
     * https://ocw.mit.edu/courses/aeronautics-and-astronautics/16-07-dynamics-fall-2009/lecture-notes/MIT16_07F09_Lec26.pdf
    **/

    double radius = centralSphere.radius;
    double motorMass = objects[1]->mass;
    double centerMass = objects[0]->mass;

    double arm = objects[2]->getPosVector().l2Metric();

    double inertialMomentX = 2.0 / 5.0 * centerMass * pow(radius, 2) + 2 * motorMass * pow(arm, 2);
    double inertialMomentY = inertialMomentX;
    double inertialMomentZ = 2.0 / 5.0 * centerMass * pow(radius, 2) + 4 * motorMass * pow(arm, 2);

    Matrix33 diagonalizedInertiaTensor = Matrix33::FromDiagonal(inertialMomentX, inertialMomentY, inertialMomentZ);

    Matrix33 transposedOrient = orientation.toMatrix().transposed();
    //inertiaTensor = orientation.toMatrix() * diagonalizedInertiaTensor * transposedOrient;
    inertiaTensor = diagonalizedInertiaTensor;

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
    /** Dynamics **/
    Quaternion dq = Quaternion::Rotation(angularVelocity, angularVelocity.l2Metric()) * deltaT;
    orientation = orientation ^ dq;

    Matrix33 omega = Matrix33::CrossProductLeft(angularVelocity);
    //Vector3dd dw = -inertiaTensor.inv() * (getMomentum() - (omega * inertiaTensor * angularVelocity));
    Vector3dd dw = inertiaTensor.inv() * (getMomentum() - (omega * inertiaTensor * angularVelocity));
    angularVelocity += dw * deltaT;

    /** Need more info about why this is needed **/
    orientation.normalise();

    /** Understood what this should do, but have no clue how to apply this to object
     * without initial angular veclocity (like DzhanibekovVideo test) **/
    //double mw = w.l2Metric();
    //angularVelocity *= mw / angularVelocity.l2Metric();

    /** Just output **/
    time_t ms = duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
    ).count();
    if(ms % 200 == 0)
    {
        //L_INFO << deltaT;
        //L_INFO<<"Delta orient: "<<orientation.getAngle()-q.getAngle();
        //L_INFO << angularAcceleration;
    }

    /** Old physics equations, could be useful **/
    //Vector3dd W = inertiaTensor.inv() * getMomentum();
    //angularAcceleration = Quaternion::pow(Quaternion::Rotation(W, W.l2Metric()), 0.000001);
    //orientation = Quaternion::pow(angularVelocity, deltaT * 1000) ^ orientation;
    //angularVelocity = Quaternion::pow(angularAcceleration, deltaT * 1000) ^ angularVelocity;
    //////////////////orientation = Quaternion::pow(angularVelocity, deltaT * 1000) ^ orientation;
    //////////////////angularVelocity = Quaternion::pow(angularAcceleration, deltaT * 1000) ^ angularVelocity;
}

DroneObject::~DroneObject()
{
    delete_safe(bodyMesh);
    for (size_t i = 0; i < motors.size(); i++)
    {
        delete_safe(motors[i].motorMesh);
        delete_safe(motors[i].propMesh);
    }
}

void DroneObject::visualTick()
{
    for (size_t i = 0; i < motors.size(); i++)
    {
        //L_INFO<<"motor: "<< i <<"; pwm: " << motors[i].pwm;
        motors[i].phi += (motors[i].cw ? motors[i].pwm : -motors[i].pwm) / 50.0;
    }
}

