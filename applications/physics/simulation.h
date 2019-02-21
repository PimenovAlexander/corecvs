#ifndef SIMULATION_H
#define SIMULATION_H
#include "bits/stdc++.h"
#include "list"
#include "iostream"

#include <bits/stl_list.h>

class Simulation
{
public:
    Simulation();
    std::list<SimObject> Objects;

    void Start();
};

#endif // SIMULATION_H
