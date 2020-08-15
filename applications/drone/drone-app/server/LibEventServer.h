//
// Created by dio on 06.12.2019.
//

#ifndef LIBEVENTAPP_LIBEVENTSERVER_H
#define LIBEVENTAPP_LIBEVENTSERVER_H

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

#include <QApplication>
#include <QObject>

const int DEFAULT_PORT = 8040;
const char *DEFAULT_IP_ADDRESS = "0.0.0.0";

class LibEventServer: public QObject {
    Q_OBJECT
private:
    evhttp *server;
    event_base *base;

public:
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

    /**
     * Initialize libEventServer
     * @return 0 on success, error code otherwise
     */
    int setup() {
        // Creating EVENT_BASE object, which is required for almost all of LibEvent's functionality
        if (!(base = event_base_new())) {
            std::cerr << "Event_base_new failed" << std::endl;
            return -1;
        }

        // Creating EVENT_HTTP object, which is responsible for routing as well as serving http content
        server = evhttp_new(base);
        if (evhttp_bind_socket(server, options.addr, options.port)) {
            std::cerr << "Binding failed" << std::endl;
            return -2;
        }
    }

    /**
     * Set a callback for a specified URI
     * @param path the path for which to invoke the callback
     * @param callback the callback function that gets invoked on specified URI
     * @return 0 on success, -1 if the callback existed already, -2 on failure
     */
    int set_callback(const char *path, void (*callback)(evhttp_request *, void *)) {
        return evhttp_set_cb(server, path, callback, nullptr);
    }

    /**
     * Set default callback
     * Sets a callback for all requests that weren't match to any of the existing callbacks
     * @param callback the callback function that gets invoked by default if no other callback matches request
     */
    void set_default_callback(void (*callback)(evhttp_request *, void *)) {
        evhttp_set_gencb(server, callback, nullptr);
    }

public slots:
    /**
     * Process all pending requests
     */
    void process_requests() {
        event_base_loop(base, EVLOOP_NONBLOCK);
    }
};

#endif //LIBEVENTAPP_LIBEVENTSERVER_H
