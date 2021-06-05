#include <fstream>
#include <bits/unique_ptr.h>

#include "libEventServer.h"
#include "base64.h"
#include "cStyleExample.h"
#include "imageGenerator.h"
#include "statsGenerator.h"

#include "core/utils/global.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/rgb24Buffer.h"



using namespace corecvs;

void on_get_index(struct evhttp_request *req, void *arg)
{
    SYNC_PRINT(("on_get_index(struct evhttp_request *req, void *arg) : called\n"));

    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    // Access html using file stream
    std::ifstream fin("pages/index.html", std::ios::in | std::ios::binary);
    if (!fin) {
        evhttp_send_reply(req, HTTP_NOTFOUND, "404 Error", evb);
        evbuffer_free(evb);
        return;
    }

    std::ostringstream oss;
    oss << fin.rdbuf();
    std::string data(oss.str());

    // Add image to buffer as base64 string
    evbuffer_add_printf(evb, "%s", data.c_str());

    evhttp_send_reply(req, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}

void on_get_test_data(struct evhttp_request *req, void *arg)
{
    SYNC_PRINT(("on_get_test_data(struct evhttp_request *req, void *arg) : called\n"));
    const char *response =
    "<html>\n"
    "<body>"
    "<h1>This is an internal test page</h1>\n"
    "This is an internal test page content\n"
    "<h2>Here is an example image</h2>\n"
    "<img src=\"test_img.bmp\" >\n"
    "</body>"
    "</html>\n";


    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    // Add content to buffer
    evbuffer_add_printf(evb, "%s", response);
    evhttp_send_reply(req, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}

void on_get_test_image(struct evhttp_request *req, void *arg)
{
    SYNC_PRINT(("on_get_test_image(struct evhttp_request *req, void *arg):called\n"));
    std::unique_ptr<RGB24Buffer> buffer(new RGB24Buffer(100, 100));
    buffer->checkerBoard(10, RGBColor::Cyan(), RGBColor::Yellow());

    /* We can save jpeg, gif, png here depending on the requested resolution, as well as apply scaling */
    vector<unsigned char> mem_buffer;
    BMPLoaderBase().save(mem_buffer, buffer.get());

    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned


    // Add image to buffer
    evbuffer_add(evb, mem_buffer.data(), mem_buffer.size());
    evhttp_add_header(req->output_headers, "Content-Type", "image/bmp");

    evhttp_send_reply(req, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}

void on_image_request(struct evhttp_request *req, void *arg)
{
#ifdef display_performance_measurements
    auto start = std::chrono::steady_clock::now();
#endif

    SYNC_PRINT(("on_image_request(struct evhttp_request *req, void *arg):called\n"));


    const char *img_name = generateImage();
    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    // Access image using file stream
    std::ifstream fin(img_name, std::ios::in | std::ios::binary);
    std::ostringstream oss;
    oss << fin.rdbuf();
    std::string data(oss.str());

    // Add image to buffer as base64 string
    evbuffer_add_printf(evb, "%s", base64_encode(data).c_str());

    evhttp_add_header(req->output_headers, "Content-Type", "image/jpeg");

    evhttp_send_reply(req, HTTP_OK, "OK", evb);
    evbuffer_free(evb);

#ifdef display_performance_measurements
    auto finish = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << std::endl;
#endif
}

void on_get_stats_request(struct evhttp_request *req, void *arg)
{
    SYNC_PRINT(("on_get_stats_request(struct evhttp_request *req, void *arg):called\n"));


    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    evbuffer_add_printf(evb, "Parameters_Changed@%i$%i$%i@%i$%i$%i",
                        getRoll(), getPitch(), getYaw(),
                        getParam1(), getParam2(), getParam3());

    evhttp_send_reply(req, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}

#ifdef poll_in_C_style
void on_long_poll_request_announce(struct evhttp_request *req, void *arg)
{
    if (arg == nullptr) {
        SYNC_PRINT(("on_long_poll_request_announce(struct evhttp_request *req, void *arg):called with NULL arg\n"));
        return;
    }

    auto poll = (LongPoll *)(arg);

    poll->announce("CLIENT_REQUEST");

    evhttp_send_reply(req, HTTP_OK, "OK", nullptr);
}

void on_long_pull_request_subscribe(struct evhttp_request *req, void *arg)
{
    if (arg == nullptr) {
        SYNC_PRINT(("on_long_pull_request_subscribe(struct evhttp_request *req, void *arg):called with NULL arg\n"));
        return;
    }

    auto poll = (LongPoll *)(arg);

    poll->subscribe("CLIENT_REQUEST", req);
}
#endif

void on_change_stats_request(struct evhttp_request *req, void *arg)
{
    SYNC_PRINT(("on_change_stats_request(struct evhttp_request *req, void *arg):called\n"));

    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    char *param;
    char *value;

    strtok(req->uri, "?");

    if ((param = strtok(NULL, "?")) != nullptr) {
        if ((value = strtok(NULL, "?")) != nullptr) {
            switch(atoi(param))
            {
                case 1: setParam1(atoi(value)); break;
                case 2: setParam2(atoi(value)); break;
                case 3: setParam3(atoi(value)); break;
                default:
                    evhttp_send_reply(req, HTTP_BADREQUEST, "Wrong parameter", evb);
                    evbuffer_free(evb);
                    return;
            }
            evhttp_send_reply(req, HTTP_OK, "OK", evb);
            evbuffer_free(evb);
            return;
        }
    }

    evhttp_send_reply(req, HTTP_BADREQUEST, "No parameters", evb);
    evbuffer_free(evb);
}

void on_other_requests(struct evhttp_request * req, void *arg)
{
    SYNC_PRINT(("on_other_requests(struct evhttp_request *req, void *arg):called\n"));

    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    evbuffer_add_printf(evb, "Page not found\n404\n");
    evhttp_send_reply(req, HTTP_NOTFOUND, "NOT_FOUND", evb);
    evbuffer_free(evb);
}
