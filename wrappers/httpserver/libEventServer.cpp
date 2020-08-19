
#include "libEventServer.h"

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
    server = evhttp_new(base);
    if (evhttp_bind_socket(server, options.addr, options.port)) {
        std::cerr << "Binding failed" << std::endl;
        return -2;
    }
    return 0;
}

/**
 * Set a callback for a specified URI
 * @param path the path for which to invoke the callback
 * @param callback the callback function that gets invoked on specified URI
 * @return 0 on success, -1 if the callback existed already, -2 on failure
 */
int LibEventServer::set_callback(const char *path, void (*callback)(evhttp_request *, void *)) {
    return evhttp_set_cb(server, path, callback, nullptr);
}

/**
 * Set default callback
 * Sets a callback for all requests that weren't match to any of the existing callbacks
 * @param callback the callback function that gets invoked by default if no other callback matches request
 */
void LibEventServer::set_default_callback(void (*callback)(evhttp_request *, void *)) {
    evhttp_set_gencb(server, callback, nullptr);
}

/**
 * Process all pending requests
 */
void LibEventServer::process_requests() {
    event_base_loop(base, EVLOOP_NONBLOCK);
}

/**
 * Remove a callback for a specified URI
 * @param path the path for which to remove the callback
 */
void LibEventServer::remove_callback(const char *path) {
    evhttp_del_cb(server, path);
}

/**
 * Set a callback for a specified URI (Overrides a callback for specified URI if it already exists)
 * @param path the path for which to invoke the callback
 * @param callback the callback function that gets invoked on specified URI
 */
void LibEventServer::override_callback(const char *path, void (*callback)(evhttp_request *, void *))
{
    if (set_callback(path, callback) == -1)
    {
        remove_callback(path);
        set_callback(path, callback);
    }
}
