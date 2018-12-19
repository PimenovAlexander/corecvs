#ifndef CONTROLLRECORD_H
#define CONTROLLRECORD_H
#include<list>

using namespace std;

class ControllRecord
{
public:
    ControllRecord();

    void Save();
    void add_message(int throttle, int roll, int yaw, int pitch );
private:
    struct message {int throttle; int roll; int yaw; int pitch ; int count_of_repeats;   } ;
    std::list<message> messages;
    message lastMessage;
    int count=0;
};

#endif // CONTROLLRECORD_H
