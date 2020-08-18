#ifndef LIBEVENTAPP_LIBEVENTSERVER_H
#define LIBEVENTAPP_LIBEVENTSERVER_H
//
// Created by dio on 06.12.2019.
//

#include <iostream>
#include <getopt.h>
#include <sys/stat.h>

#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>

#include <evhttp.h>
#include <string>
#include <string.h>
#include <cstring>

class LibEventServer {
private:
    evhttp *server;
    event_base *base;

public:
    const int DEFAULT_PORT = 8040;
    const char *DEFAULT_IP_ADDRESS = "0.0.0.0";

    struct _options {
        int port;
        char *addr;
        int verbose;
    } options;

    LibEventServer() {
        options.port = DEFAULT_PORT;
        options.addr = (char *) DEFAULT_IP_ADDRESS;
        options.verbose = 0;
    }

    int setup();
    int set_callback(const char *path, void (*callback)(evhttp_request *, void *));
    void set_default_callback(void (*callback)(evhttp_request *, void *));
    void process_requests();
};

#endif //LIBEVENTAPP_LIBEVENTSERVER_H
