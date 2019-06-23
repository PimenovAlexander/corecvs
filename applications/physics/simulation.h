#ifndef SIMULATION_H
#define SIMULATION_H
#include "bits/stdc++.h"
#include "list"
#include "iostream"
#include "simObject.h"
#include "simSphere.h"
#include "mainObject.h"
#include <bits/stl_list.h>

class Simulation
{
public:
    Simulation();
    vector<MainObject> mainObjects;

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
