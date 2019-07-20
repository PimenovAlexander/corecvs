#ifndef SIMULATION_H
#define SIMULATION_H
#include "bits/stdc++.h"
#include "list"
#include "iostream"
#include "physObject.h"
#include "physSphere.h"
#include "physMainObject.h"
#include <bits/stl_list.h>
#include "copter/droneObject.h"
#include "sceneShaded.h"
#include "joystickInput.h"
class Simulation
{
public:
    Simulation();
    vector<PhysMainObject> mainObjects;
    DroneObject drone;
    std::chrono::high_resolution_clock::time_point oldTime;
    std::chrono::high_resolution_clock::time_point newTime;
    std::chrono::high_resolution_clock::time_point startTime;

    std::chrono::high_resolution_clock::time_point endTime;
    std::chrono::duration<double> time_span;
    SceneShaded *mShadedScene = NULL;
    int frameCounter=0;
    void start();
    Simulation(std::string arg);

    CopterInputs droneJoystick;

    void startRealTimeSimulation();
    void execTestSimulation();

    bool getIsAlive();
    void setIsAlive(const bool flag);

private:
    void defaultStart();
    void droneStart();
    std::mutex simMutex;
    bool isAlive = false;

};
#endif // SIMULATION_H
