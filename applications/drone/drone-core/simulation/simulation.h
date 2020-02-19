#ifndef SIMULATION_H
#define SIMULATION_H

#include <bits/stdc++.h>
#include <bits/stl_list.h>
#include <list>
#include <iostream>
#include <time.h>

#include "copter/droneObject.h"
#include "physicsObject.h"
#include "physicsSphere.h"
#include "physicsCompoundObject.h"
#include "3d/sceneShaded.h"

class Simulation
{
public:
    Simulation();
    vector<PhysicsMainObject> mainObjects;

    DroneObject drone;

    /* remove this ASAP */
#if 0
    std::chrono::high_resolution_clock::time_point oldTime;
    std::chrono::high_resolution_clock::time_point newTime;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point noiseTime;
    std::chrono::high_resolution_clock::time_point noiseReverseTime;

    std::chrono::high_resolution_clock::time_point endTime;
    std::chrono::duration<double> time_span;
    std::chrono::duration<double> time_since_start;
#endif

    SceneShaded *mShadedScene = NULL;
    int frameCounter=0;
    void start();
    Simulation(std::string arg);

    CopterInputs droneJoystick;

    void startRealTimeSimulation();
    void execTestSimulation();

    bool getIsAlive();
    void setIsAlive(const bool flag);

    //Test
    std::vector<Vector3dd> testForces;
    std::vector<size_t> time_of_forces;
    std::vector<size_t> time_between_forces;

    void startDroneSimulation();
    //void execTestPhysObject();
    //void execJanibekovTest();

private:
    void defaultStart();
    void droneStart();
    std::mutex simMutex;
    bool isAlive = false;
    bool noiseFlag = true;
    bool outputFlag = true;
};
#endif // SIMULATION_H
