#ifndef LIBEVENTAPP_SERVER_H
#define LIBEVENTAPP_SERVER_H

//
// Created by dio on 7/26/20.
//


#include "libEventServer.h"


/**
 * This is an extended server version that is oriented on modules and contents
 **/
class HttpServer : public LibEventServer {
public:
    HttpServer(int port = DEFAULT_PORT, const char *addr = (const char *) DEFAULT_IP_ADDRESS, int verbose = 0) :
        LibEventServer(port, addr, verbose)
    {}

    virtual int setup() override;

    /* This method starts the server in a separate thread */
    /* You may want to start it synchroniously, then call LibEventServer methods directly */
    void start();

};


#endif //LIBEVENTAPP_SERVER_H
