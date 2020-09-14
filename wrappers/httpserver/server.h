#ifndef LIBEVENTAPP_SERVER_H
#define LIBEVENTAPP_SERVER_H


#include <thread>
#include <vector>

#include "libEventServer.h"


#include <httpServerModule.h>



/**
 * This is an extended server version that is oriented on modules and contents
 **/
class HttpServer : public LibEventServer {
public:
    std::vector<HttpServerModule *> mModuleList;

    LongPoll *poll = nullptr;

    HttpServer(int port = DEFAULT_PORT, const char *addr = (const char *) DEFAULT_IP_ADDRESS, int verbose = 0) :
        LibEventServer(port, addr, verbose),
        poll(new LongPoll)
    {}

    int setup() override;
    virtual void httpCallback(evhttp_request *request);

    static  void httpCallbackStatic(evhttp_request *request, void *server);


    /* This method starts the server in a separate thread */
    /* You may want to start it synchroniously, then call LibEventServer methods directly */
    std::thread *serverThread = nullptr;
    void start();

    int addEvent(const std::string& name, ContentProvider *provider);
    int addEvent(const std::string& name, const std::string& url, ContentProvider *provider);

    static void runStatic(HttpServer *server);
    [[noreturn]] void run();


    void addModule(HttpServerModule *module);

    ~HttpServer();

};


#endif //LIBEVENTAPP_SERVER_H
