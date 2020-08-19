#include <fstream>

#include "core/reflection/commandLineSetter.h"
#include "core/fileformats/meshLoader.h"
#include "core/fileformats/plyLoader.h"
#include "core/utils/utils.h"


#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif

#define resource_file_enabled
#ifdef resource_file_enabled
extern "C" const char jojo0_jpg[];
extern "C" const size_t jojo0_jpg_size;
#endif

#include "server.h"

using namespace corecvs;
using namespace std;

#ifdef resource_file_enabled
void on_resource_image_request(struct evhttp_request *req, void *arg)
{
    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    // Add image to buffer
    evbuffer_add(evb, jojo0_jpg, jojo0_jpg_size);
    evhttp_add_header(req->output_headers, "Content-Type", "image/jpg");

    evhttp_send_reply(req, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}
#endif

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

    SYNC_PRINT(("Starting web server...\n"));

    startWServer();

#ifdef resource_file_enabled
    std::cout << "Size of an image : " << jojo0_jpg_size << std::endl;
    server->set_callback("/test_img.jpg", on_resource_image_request);
#endif

    while (true) {
        usleep(10);
        server->process_requests();
    }
    SYNC_PRINT(("Exiting web server...\n"));
    return 0;
}

