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
class Simulation
{
public:
    Simulation();
    vector<PhysMainObject> mainObjects;
    DroneObject drone;
    std::chrono::high_resolution_clock::time_point oldTime;
    std::chrono::high_resolution_clock::time_point newTime;
    std::chrono::high_resolution_clock::time_point startTime;

    int frameCounter=0;
    void start();
    Simulation(std::string arg);

private:
    void defaultStart();
    void droneStart();
};

#endif // SIMULATION_H
