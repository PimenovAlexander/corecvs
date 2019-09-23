#include "simulation.h"
#include "list"
#include "simObject.h"
#include "simSphere.h"
#include <ctime>
#include "core/utils/log.h"
#include <chrono>
#include "mesh3DScene.h"
#include "physicsMainWindow.h"
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

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void Simulation::startRealTimeSimulation()
{

    std::thread thr([this]()
    {

        oldTime = std::chrono::high_resolution_clock::now();
        while (isAlive)
        {
           drone.flightControllerTick(droneJoystick);
           newTime = std::chrono::high_resolution_clock::now();
           time_span = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-oldTime);
           drone.physicsTick(time_span.count());
           oldTime=newTime;
        }
    });
    thr.detach();
}

void Simulation::execJanibekovTest()
{ 
    std::thread thr([this]()
    {
        srand(NULL);
        startTime = std::chrono::high_resolution_clock::now();
        oldTime = startTime;
        noiseTime = startTime;
        noiseReverseTime = startTime;
        while (isAlive)
        {

            newTime = std::chrono::high_resolution_clock::now();

            double timePassed = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-startTime).count();

            Affine3DQ motorToWorld = testBolt.getTransform() * testBolt.partsOfSystem[1].getPosAffine();
            Matrix33 transposedOrient = motorToWorld.rotor.toMatrix();
            transposedOrient.transpose();
            Vector3dd force = transposedOrient * Vector3dd(0.0, 0.0, 0.1);

            if(timePassed > 5 && timePassed < 6)
            {
                testBolt.partsOfSystem[1].addForce(force);
                testBolt.partsOfSystem[0].addForce(-force);
            }

            if(timePassed < 1)
            {
                testBolt.partsOfSystem[2].addForce(Vector3dd(0.0, 0.0, 0.1));
            }

            if(timePassed > 1 && timePassed < 2)
            {
                testBolt.partsOfSystem[2].addForce(Vector3dd(0.0, 0.0, -0.1));
            }

            if(noiseFlag)
            {
                noiseTime = std::chrono::high_resolution_clock::now();
                //testBolt.partsOfSystem[2].addForce(Vector3dd(0.0, 0.0, fRand(5,10)));
                if(std::chrono::duration_cast<std::chrono::duration<double>>(noiseTime - noiseReverseTime).count() > 0.5)
                {
                    noiseFlag = !noiseFlag;
                }
            }
            else
            {
                noiseReverseTime = std::chrono::high_resolution_clock::now();
                //testBolt.partsOfSystem[2].addForce(Vector3dd(0.0, 0.0, -fRand(5,10)));
                if(std::chrono::duration_cast<std::chrono::duration<double>>(noiseReverseTime - noiseTime).count() > 0.5)
                {
                    noiseFlag = !noiseFlag;
                }
            }

            time_span = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-oldTime);
            testBolt.physicsTick(time_span.count());
            oldTime=newTime;

            testBolt.startTick();
        }
    });
    thr.detach();
}

void Simulation::execTestSimulation()
{
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
        startTime = std::chrono::high_resolution_clock::now();
        drone.testMode = true;
        oldTime = std::chrono::high_resolution_clock::now();
        while (isAlive)
        {
           drone.motors[1].addForce(Vector3dd(0,0,0.01));
           drone.motors[0].addForce(Vector3dd(0,0,0.01));
           //drone.flightControllerTick(droneJoystick);
           newTime = std::chrono::high_resolution_clock::now();
           time_span = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-oldTime);
           drone.physicsTick(time_span.count());
           oldTime=newTime;

           drone.startTick();
        }
    });
    thr.detach();
/*
    drone.flightControllerTick(droneJoystick);

    //deltaT here is wrong
    drone.physicsTick(0.1);
*/
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
    PhysMainObject *mainObject = &mainObjects.back();
    mainObject->countPhysics = true;
    double radius = 2.0;
    double mass = 1.0;
    Affine3DQ pos1 = Affine3DQ(Vector3dd(-1, -1, -1));
    Affine3DQ pos2 = Affine3DQ(Vector3dd(1, -1, -1));
    Affine3DQ pos3 = Affine3DQ(Vector3dd(-1, 1, -1));
    Affine3DQ pos4 = Affine3DQ(Vector3dd(1, 1, -1));
    PhysSphere sphere1 = PhysSphere(&pos1, &radius, &mass);
    PhysSphere sphere2 = PhysSphere(&pos2, &radius, &mass);
    PhysSphere sphere3 = PhysSphere(&pos3, &radius, &mass);
    PhysSphere sphere4 = PhysSphere(&pos4, &radius, &mass);
    mainObject->addObject(&sphere1);
    mainObject->addObject(&sphere2);
    mainObject->addObject(&sphere3);
    mainObject->addObject(&sphere4);
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
    startTime = std::chrono::high_resolution_clock::now();
    oldTime = std::chrono::high_resolution_clock::now();

    std::thread thr([this]()
    {
        cout<<mainObjects.size()<<" after thread"<<endl;
        cout<<"kek"<<endl;
        while (isAlive)
        {
            newTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-oldTime);
            std::chrono::duration<double> currentTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-startTime);

            // cout<<time_span.count()<<endl;
            for (int i=0; i<mainObjects.size(); i++)
            {
                mainObjects[i].tick(time_span.count());
             }
            frameCounter++;
            oldTime=newTime;
            usleep(3000);
        }
    });
    thr.detach();
}
