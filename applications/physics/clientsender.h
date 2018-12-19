#ifndef CLIENTSENDER_H
#define CLIENTSENDER_H
#include <iostream>



class ClientSender
{
public:
    ClientSender();
    int port;
    bool connected=false;
    void Client_connect();
    int s;
    void Client_send(std::string);
};

#endif // CLIENTSENDER_H
