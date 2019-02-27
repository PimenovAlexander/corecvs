#include "simulation.h"
#include "list"
#include "simObject.h"
#include "simSphere.h"
#include <ctime>

#include <chrono>

Simulation::Simulation()
{
    //objects2 =  std::list<SimObject>();
    SimSphere kura;
    kura.countPhysics=true;
    spheres.push_back(kura);
    objects.push_back(&spheres[0]);
    cout<<objects.size()<<" before thread"<<endl;
}
void Simulation::start()
{
    startTime = std::chrono::high_resolution_clock::now();
    oldTime = std::chrono::high_resolution_clock::now();
    cout<<objects.size()<<" before thread v2"<<endl;

    std::thread thr([this]()
    {
        cout<<objects.size()<<" after thread"<<endl;
        cout<<"kek"<<endl;
        while (true)
        {
            newTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-oldTime);
            std::chrono::duration<double> currentTime = std::chrono::duration_cast<std::chrono::duration<double>>(newTime-startTime);

            // cout<<time_span.count()<<endl;
            for (int i=0;i<objects.size();i++)
            {
                objects[i]->tick(time_span.count());
                objects[i]->saveMesh( std::to_string(currentTime.count()));
            }
            cout<<objects[0]->coords<<endl;
            frameCounter++;

            /*if (frameCounter%1000==0)

            {
                cout<<"counter - "<<frameCounter<<endl;

            }*/
            oldTime=newTime;
            usleep(3000);
        }
    });
    thr.detach();
}
