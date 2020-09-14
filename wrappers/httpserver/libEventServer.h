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

#include <longPoll.h>

#include <evhttp.h>
#include <string>
#include <string.h>
#include <cstring>

#include <memory>

/**
 * Base class for callback driven interface for HTTP server
 **/
class LibEventServer {
private:
    evhttp     *server = nullptr;
    event_base *base   = nullptr;

public:
    static const int   DEFAULT_PORT;
    static const char *DEFAULT_IP_ADDRESS;

    struct Options {
        Options(int port, const char *addr, int verbose) :
            port(port),
            addr(addr),
            verbose(verbose)
        {}

        int port;
        std::string addr;
        int verbose;
    };

    Options options;

    LibEventServer( int port = DEFAULT_PORT, const char *addr = (const char *) DEFAULT_IP_ADDRESS, int verbose = 0) :
        options(port, addr, verbose)
    {
    }

    virtual int setup();

    int set_callback(const char *path, void (*callback)(evhttp_request *, void *), void *arg = NULL);
    void remove_callback(const char *path);
    void override_callback(const char *path, void (*callback)(evhttp_request *, void *), void *arg = NULL);

    void set_default_callback(void (*callback)(evhttp_request *, void *), void *arg = NULL);
    void process_requests(bool nonBlock = true);
};

#endif //LIBEVENTAPP_LIBEVENTSERVER_H
