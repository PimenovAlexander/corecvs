#include "simulation.h"
#include "list"
#include "simobject.h"

Simulation::Simulation()
{
    Objects =  std::list<SimObject>();

}
void Simulation::Start()
{
    std::thread thr([this]()
    {
        for (SimObject obj:Objects)
        {
            if (obj.CountPhysics)
            {

            }
        }
    });
    thr.detach();
}
