#include <list>

#include "core/utils/log.h"

#include "simulation.h"
#include "simObject.h"
#include "simSphere.h"
#include "3d/mesh3DScene.h"

using namespace std;
using namespace corecvs;

Simulation::Simulation()
{
    defaultStart();
}

Simulation::Simulation(string arg)
{
    if (arg=="drone")
    {

    }
    else
    {
        defaultStart();
    }
}

void Simulation::startRealTimeSimulation()
{
    drone.flightControllerTick(droneJoystick);
    drone.physicsTick(1.0);
}

#if 0
void Simulation::execJanibekovTest()
{
    std::thread thr([this]()
    {
        srand(NULL); /*Was is das? */
        //noiseTime = startTime;
        //noiseReverseTime = startTime;

        //Quaternion testAngVel = Quaternion(-0.00144962, -2.8152e-11, 9.80112e-15, 0.999999);
        Vector3dd testAngVel = Vector3dd(1, 0.01, 0); //* 0.000001;
        //Quaternion testOrientation = Quaternion(0, 0.012489, 0, 0.999922);
        Quaternion testOrientation = Quaternion::Identity();
        L_INFO << "INTIAL ORIENTATION: " << testOrientation;
        testBolt. = testOrientation;
        testBolt.w = testAngVel;

        testBolt.mw = testAngVel.l2Metric();

        startTime = std::chrono::high_resolution_clock::now();
        oldTime = startTime;

        while (isAlive)
        {

            //double timePassed = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-startTime).count();

            // TODO: Fixme
            //Affine3DQ motorToWorld = testBolt.getTransform() * testBolt.partsOfSystem[1].getPosAffine();

            //Matrix33 transposedOrient = motorToWorld.rotor.toMatrix();
            //transposedOrient.transpose();

            //Vector3dd force = transposedOrient * Vector3dd(0.0, 0.0, 0.05);
            //Vector3dd force2 = Vector3dd(0.0, 0.0, 0.03);
            //Quaternion q = Quaternion(-0.00744148, -8.46662e-11, 0.000934261, 0.999972);

            newTime = std::chrono::high_resolution_clock::now();
            time_span = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - oldTime);
            time_since_start = std::chrono::duration_cast<std::chrono::duration<double>>(newTime - startTime);

            if(time_since_start.count() < 2 * M_PI)
            {
                testBolt.physicsTick(time_span.count());
                oldTime=newTime;
                testBolt.startTick();
            }
            else
            {
                if(outputFlag)
                {
                    L_INFO << "FINAL ORIENTATION: " << testBolt.orientation;
                    outputFlag = false;
                }
            }
        }
    });
    thr.detach();
}
#endif

void Simulation::startDroneSimulation()
{
#if 0
    std::thread thr([this]()
    {
        //srand(NULL); /*< Was ist das? */

        startTime = std::chrono::high_resolution_clock::now();
        oldTime = startTime;
        noiseTime = startTime;
        noiseReverseTime = startTime;

        while (isAlive)
        {
            newTime = std::chrono::high_resolution_clock::now();
            double timePassed = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-startTime).count();

            Affine3DQ motorToWorld = testBolt.getTransform() * testBolt.partsOfSystem[1]->affine();
            Matrix33 transposedOrient = motorToWorld.rotor.toMatrix();
            transposedOrient.transpose();

            time_span = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-oldTime);
            testBolt.physicsTick(time_span.count());
            oldTime=newTime;

            testBolt.startTick();
        }
    });
    thr.detach();
#endif
}


void Simulation::execTestSimulation()
{
#if 0
    srand(time(NULL));
    for (int i = 0; i < 4; i++)
    {
        Vector3dd f = Vector3dd(fRand(1, 10), fRand(1, 10), fRand(1, 10));
        double t = fRand(1, 2) * 1000;
        testForces.push_back(f);
        time_of_forces.push_back((size_t)std::trunc(t));
    }

    for (int i = 0; i < 7; i++)
    {
        double t_between = fRand(3, 5) * 1000;
        time_between_forces.push_back((size_t)std::trunc(t_between));
    }

    for (int i = 0;i<7;i++) {
        if(i<4)
            L_INFO <<"Force: "<< testForces[i]<<" Time of Force: " << time_of_forces[i]<< " Time Between: " << time_between_forces[i];
        else
            L_INFO <<" Time Between: " << time_between_forces[i];
    }

    std::thread thr([this]()
    {
        drone.testMode = true;
        while (isAlive)
        {
           drone.motors[1]->addForce(Vector3dd(0, 0, 1) * 0.1);
           drone.motors[0]->addForce(Vector3dd(0, 0, 1) * 0.1);
           drone.physicsTick(time_span.count());
           drone.startTick();
        }
    });
    thr.detach();
/*
    drone.flightControllerTick(droneJoystick);

    //deltaT here is wrong
    drone.physicsTick(0.1);
*/
#endif
}

bool Simulation::getIsAlive()
{
    simMutex.lock();
    bool b = isAlive;
    simMutex.unlock();
    return b;
}

void Simulation::setIsAlive(const bool flag)
{
    simMutex.lock();
    isAlive = flag;
    simMutex.unlock();
}

void Simulation::droneStart()
{
    mainObjects.emplace_back();
    PhysicsMainObject *mainObject = &mainObjects.back();
    mainObject->countPhysics = true;
    double radius = 2.0;
    double mass = 1.0;
    Affine3DQ pos1 = Affine3DQ(Vector3dd(-1, -1, -1));
    Affine3DQ pos2 = Affine3DQ(Vector3dd(1, -1, -1));
    Affine3DQ pos3 = Affine3DQ(Vector3dd(-1, 1, -1));
    Affine3DQ pos4 = Affine3DQ(Vector3dd(1, 1, -1));
    PhysicsSphere *sphere1 = new PhysicsSphere(pos1, radius, mass);
    PhysicsSphere *sphere2 = new PhysicsSphere(pos2, radius, mass);
    PhysicsSphere *sphere3 = new PhysicsSphere(pos3, radius, mass);
    PhysicsSphere *sphere4 = new PhysicsSphere(pos4, radius, mass);
    mainObject->addObject(sphere1);
    mainObject->addObject(sphere2);
    mainObject->addObject(sphere3);
    mainObject->addObject(sphere4);
    mainObject->addForce(Vector3dd(0,-9.8,0));


    cout << "Simulation::Simulation():" << mainObjects[0].objects.size() << " before thread" << endl;
}

void Simulation::defaultStart()
{
    /** Adds new MainObject to the vector **/
    /*
    mainObjects.emplace_back();
    PhysMainObject *mainObject = &mainObjects.back();
    mainObject->countPhysics = true;
    double radius = 2.0;
    double mass = 1.0;
    Affine3DQ pos1 = Affine3DQ(Vector3dd(-1, -1, -1));
    PhysSphere sphere1 = PhysSphere(&pos1, &radius, &mass);
    mainObject->addObject(&sphere1);
    mainObject->addForce(Vector3dd(0,-9.8,0));
    cout << "Simulation::Simulation():" << mainObjects[0].objects.size() << " before thread" <<endl;
    */

}



void Simulation::start()
{
    /* Use PreciseTime instead of chrono, it could make code a bit more compact */

    std::thread thr([this]()
    {
        cout<<mainObjects.size()<<" after thread"<<endl;
        while (isAlive)
        {
            for (int i = 0; i < mainObjects.size(); i++)
            {
                mainObjects[i].tick(0.1);
             }
            frameCounter++;
        }
    });
    thr.detach();
}
