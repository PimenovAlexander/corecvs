#ifndef CONTROLLRECORD_H
#define CONTROLLRECORD_H
#include<list>

using namespace std;

class ControllRecord
{
public:
    ControllRecord();

    void save();
    void addMessage(int throttle, int roll, int yaw, int pitch );
private:
    struct Message {int throttle; int roll; int yaw; int pitch; int countOfRepeats; };
    std::list<Message> messages;
    Message lastMessage;
    int count=0;
};

#endif // CONTROLLRECORD_H
