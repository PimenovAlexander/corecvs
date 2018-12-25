#include "controllrecord.h"
#include <iostream>
#include <fstream>



    ControllRecord::ControllRecord()
    {
        lastMessage.pitch=-1;
        lastMessage.roll=-1;
        lastMessage.yaw=-1;
        lastMessage.throttle=-1;
        lastMessage.count_of_repeats=-1;

    }

    void ControllRecord::Save()
    {
        messages.push_back(lastMessage);
        count++;
        ofstream fout("cppstudio.txt");

        for (message m : messages)
        {
             cout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.count_of_repeats<<endl;
            fout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.count_of_repeats<<endl;
         }
        fout.close();
        cout<<count<<"-count "<<messages.size()<<"-size"<<endl;
    }

    void ControllRecord::add_message(int throttle, int roll, int yaw, int pitch )
    {
        if ( lastMessage.pitch==pitch && lastMessage.yaw==yaw && lastMessage.roll==roll && lastMessage.throttle == throttle  )
        {
            lastMessage.count_of_repeats++;
        }
        else
        {
             messages.push_back(lastMessage);          //add last_message
             lastMessage.yaw=yaw;
             lastMessage.throttle=throttle;
             lastMessage.pitch=pitch;
             lastMessage.roll=roll;
             lastMessage.count_of_repeats=0;
             count++;
        }
    }
