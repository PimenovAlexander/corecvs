#include "libEventServer.h"

const int   LibEventServer::DEFAULT_PORT = 8040;
const char *LibEventServer::DEFAULT_IP_ADDRESS = "0.0.0.0";


/**
     * Initialize libEventServer
     * @return 0 on success, error code otherwise
     */
int LibEventServer::setup() {
    // Creating EVENT_BASE object, which is required for almost all of LibEvent's functionality


    if (!(base = event_base_new())) {
        std::cerr << "Event_base_new failed" << std::endl;
        return -1;
    }

    // Creating EVENT_HTTP object, which is responsible for routing as well as serving http content
    if (options.verbose > 0) {
        SYNC_PRINT(("LibEventServer::setup(): called for %s:%d\n", options.addr.c_str(), options.port));
    }


    server = evhttp_new(base);
    if (evhttp_bind_socket(server, options.addr.c_str(), options.port)) {
        std::cerr << "Binding failed" << std::endl;
        return -2;
    }

    if (options.verbose > 0) {
        SYNC_PRINT(("LibEventServer::setup(): setup finished\n"));
    }
    return 0;
}

/**
 * Set a callback for a specified URI
 * @param path the path for which to invoke the callback
 * @param callback the callback function that gets invoked on specified URI
 * @return 0 on success, -1 if the callback existed already, -2 on failure
 */
int LibEventServer::set_callback(const char *path, void (*callback)(evhttp_request *, void *), void *arg)
{
    if (options.verbose > 0) {
        SYNC_PRINT(("LibEventServer::set_callback(<%s>, _, %s): called\n", path, arg == NULL ? "null" : "non null" ));
    }

    if (server == NULL) {
        SYNC_PRINT(("LibEventServer::set_callback(): called before setup()\n"));
        return -2;
    }

    return evhttp_set_cb(server, path, callback, arg);
}

/**
 * Set default callback
 * Sets a callback for all requests that weren't match to any of the existing callbacks
 * @param callback the callback function that gets invoked by default if no other callback matches request
 */
void LibEventServer::set_default_callback(void (*callback)(evhttp_request *, void *), void *arg) {

    if (options.verbose > 0) {
        SYNC_PRINT(("LibEventServer::set_default_callback( _, %s): called\n", arg == NULL ? "null" : "non null" ));
    }

    if (server == nullptr) {
        SYNC_PRINT(("LibEventServer::set_default_callback(): called before setup()\n"));
        return;
    }
    evhttp_set_gencb(server, callback, arg);
}

/**
 * Process all pending requests
 */
void LibEventServer::process_requests(bool nonBlock) {
    if (options.verbose > 1) {
        SYNC_PRINT(("LibEventServer::process_requests(nonblock=%s): called\n", nonBlock ? "yes" : "no" ));
    }

    if (base == NULL) {
        SYNC_PRINT(("LibEventServer::process_requests(): called before setup()\n"));
        return;
    }
    event_base_loop(base, nonBlock ? EVLOOP_NONBLOCK : EVLOOP_ONCE);
}

/**
 * Remove a callback for a specified URI
 * @param path the path for which to remove the callback
 */
void LibEventServer::remove_callback(const char *path) {

    if (server == NULL) {
        SYNC_PRINT(("LibEventServer::remove_callback(): called before setup()"));
    }
    evhttp_del_cb(server, path);
}

/**
 * Set a callback for a specified URI (Overrides a callback for specified URI if it already exists)
 * @param path the path for which to invoke the callback
 * @param callback the callback function that gets invoked on specified URI
 */
void LibEventServer::override_callback(const char *path, void (*callback)(evhttp_request *, void *), void *arg)
{
    if (options.verbose > 0) {
        SYNC_PRINT(("LibEventServer::override_callback(<%s>, _, %s): called\n", path, arg == NULL ? "null" : "non null" ));
    }

    if (set_callback(path, callback, arg) == -1)
    {
        remove_callback(path);
        set_callback(path, callback, arg);
    }
}
