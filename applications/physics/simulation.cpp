#include "simulation.h"
#include "list"

Simulation::Simulation()
{
    Objects =  std::list<int>();

}
void Simulation::Start()
{
    std::thread thr([this]()
    {

    });
    thr.detach();
}
