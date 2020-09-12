#include "server.h"
#include "core/utils/global.h"
#include "core/buffers/rgb24/rgb24Buffer.h"


using namespace corecvs;

/**
     * Initialize HttpServer
     * @return 0 on success, error code otherwise
     */
int HttpServer::setup()
{
    int setupCode = LibEventServer::setup();
    if (setupCode)
        return setupCode;

    set_default_callback(HttpServer::httpCallbackStatic, this);

    return 0;
}

/**
 * Default callback that acts as a router and handles the request to an appropriate module
 */
void HttpServer::httpCallback(evhttp_request *request)
{
    if (options.verbose > 0) {
        SYNC_PRINT(("HttpServer::httpCallback(): called\n"));
    }

    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    SYNC_PRINT(("URL: <%s>\n", request->uri ));

    std::string url(request->uri);

    /* Modules */
    std::shared_ptr<HttpContent> contentPointer;

    for (const auto& module : mModuleList)
    {
        SYNC_PRINT((" - Checking module with prefix <%s>\n", module->getPrefix().c_str()));
        if (module->shouldProcess(url)) {
            contentPointer = module->getContent(url);
            if (!contentPointer) {
                continue;
            } else {
                SYNC_PRINT((" - We have a match!\n" ));
                break;
            }

#if 0
            if (mModuleList[i]->shouldWrap(url))
            {
                pointer = QSharedPointer<HttpContent>(new WrapperContent(pointer));
            }
#endif
        }
    }

    if (!contentPointer) {
        evhttp_send_reply(request, HTTP_BADREQUEST, "FAIL", evb);
    } else {
        evhttp_add_header(request->output_headers, "Content-Type", contentPointer->getContentType().c_str());

        vector<uint8_t> mem_buffer = contentPointer->getContent();
        SYNC_PRINT((" - Outputing %d bytes of reply\n", (int)mem_buffer.size() ));
        evbuffer_add(evb, mem_buffer.data(), mem_buffer.size());

        evhttp_send_reply(request, HTTP_OK, "OK", evb);
    }
    evbuffer_free(evb);
}

/**
 * Default callback that acts as a router and handles the request to an appropriate module
 */
void HttpServer::httpCallbackStatic(evhttp_request *request, void *server)
{
    HttpServer * httpServer = static_cast<HttpServer *>( server );
    httpServer->httpCallback(request);
}

/**
 * Starts an http-server in a new thread
 */
void HttpServer::start()
{
    if (options.verbose > 0) {
        SYNC_PRINT(("HttpServer::start(): called\n"));
    }
    serverThread = new std::thread(HttpServer::runStatic, this);
}

/**
 * Starts an http-server in a new thread
 */
void HttpServer::runStatic(HttpServer *server)
{
    server->run();
}

/**
 * Starts a processing loop
 */
[[noreturn]] void HttpServer::run()
{
    while (true) {
        usleep(10);
        process_requests(false);
    }
}


void HttpServer::addModule(HttpServerModule *module)
{
    mModuleList.push_back(module);
}

HttpServer::~HttpServer()
{
    if (options.verbose > 0) {
        SYNC_PRINT(("HttpServer::~HttpServer(): called. Starting shutdown\n"));
    }
}
