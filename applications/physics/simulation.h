#ifndef SIMULATION_H
#define SIMULATION_H
#include "bits/stdc++.h"
#include "list"
#include "iostream"
#include "simobject.h"

#include <bits/stl_list.h>

class Simulation
{
public:
    Simulation();
    vector<SimObject> Objects ;
    std::chrono::high_resolution_clock::time_point oldTime;
    std::chrono::high_resolution_clock::time_point newTime;
    int frameCounter=0;
    void Start();
};

#endif // SIMULATION_H
