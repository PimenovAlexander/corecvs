#ifndef CLIENTSENDER_H
#define CLIENTSENDER_H
#include <iostream>



class ClientSender
{
public:
    ClientSender();
    int port;
    bool connected=false;
    void clientConnect();
    int s;
    void clientSend(std::string);
};

#endif // CLIENTSENDER_H
