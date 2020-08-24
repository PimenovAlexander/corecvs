#include <fstream>
#include <thread>


#include "core/reflection/commandLineSetter.h"
#include "core/fileformats/meshLoader.h"
#include "core/fileformats/plyLoader.h"
#include "core/utils/utils.h"

#include "cStyleExample.h"


#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif

extern "C" const char res_jojo0_jpg[];
extern "C" const size_t res_jojo0_jpg_size;

#include "server.h"

using namespace corecvs;
using namespace std;

void on_resource_image_request(struct evhttp_request *req, void *arg)
{
    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    // Add image to buffer
    evbuffer_add(evb, res_jojo0_jpg, res_jojo0_jpg_size);
    evhttp_add_header(req->output_headers, "Content-Type", "image/jpg");

    evhttp_send_reply(req, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}

void server_loop(LibEventServer * server) {
    int count = 0;
    while (true) {
        usleep(10);
        server->process_requests();
        count++;
    }
}


int main (int argc, char **argv)
{
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif


    CommandLineSetter s(argc, argv);

#define BASIC
#ifdef BASIC
    std::thread *mThread = NULL;
    /* Start first basic webserver */
    {
        SYNC_PRINT(("Starting web server 1...\n"));
        LibEventServer *server = new LibEventServer(8040, "0.0.0.0", 1);

        server->setup();

        server->override_callback("/test_img.bmp"  , on_resource_image_request);
        server->set_callback("/"                   , on_get_index , server);

        server->set_callback("/test"               , on_get_test_data, server );
        server->set_callback("/test_img.bmp"       , on_get_test_image, server);

        server->set_callback("/image_request"      , on_image_request, server);
        server->set_callback("/stats_request"      , on_get_stats_request, server);

        server->set_callback("/long_poll"          , on_long_pull_request_subscribe, server);
        server->set_callback("/long_poll_announce" , on_long_poll_request_announce, server);

        server->set_callback("/change_stat_request", on_change_stats_request, server);
        server->set_default_callback(on_other_requests);

        server->poll->addEvent("CLIENT_REQUEST", on_get_test_image);

        /* Start server thread */
        mThread = new std::thread(server_loop, server);
    }
#endif

#ifdef MODULAR
    /* Start modular webserver */
    {
        HttpServer server;
        server.init();
        server.start();

    }

#endif

    /* Run some background task */
    int count = 0;
    while (true) {
        char animation[4] = {'|', '/', '-', '\\' };
        SYNC_PRINT(("\r%c", animation[count % 4]));
        usleep(10000);
        count++;
    }

    SYNC_PRINT(("Exiting web server...\n"));
    return 0;
}

