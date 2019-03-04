#include "controllRecord.h"
#include <iostream>
#include <fstream>
#include <linux/joystick.h>




ControllRecord::ControllRecord()
{
    lastMessage.pitch=-1;
    lastMessage.roll=-1;
    lastMessage.yaw=-1;
    lastMessage.throttle=-1;
    lastMessage.countOfRepeats=-1;

}

void ControllRecord::save()
{
    messages.push_back(lastMessage);
    count++;
    ofstream fout("cppstudio.txt");

    for (Message m : messages)
    {
        cout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.countOfRepeats<<endl;
        fout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.countOfRepeats<<endl;
    }
    fout.close();
    cout<<count<<"-count "<<messages.size()<<"-size"<<endl;
}

void ControllRecord::addMessage(int throttle, int roll, int yaw, int pitch )
{
    if ( lastMessage.pitch==pitch && lastMessage.yaw==yaw && lastMessage.roll==roll && lastMessage.throttle == throttle  )
    {
        lastMessage.countOfRepeats++;
    }
    else
    {
        messages.push_back(lastMessage);          //add last_message
        lastMessage.yaw=yaw;
        lastMessage.throttle=throttle;
        lastMessage.pitch=pitch;
        lastMessage.roll=roll;
        lastMessage.countOfRepeats=0;
        count++;
    }
}
