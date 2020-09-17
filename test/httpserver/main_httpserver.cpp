#include <fstream>
#include <graphModule.h>
#include <imageListModule.h>
#include <reflectionListModule.h>
#include <resourcePackModule.h>
#include <statisticsListModule.h>
#include <memory>
#include <thread>


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

extern "C" const char res__someOtherImages_jojo0_jpg[];
extern "C" const size_t res__someOtherImages_jojo0_jpg_size;

extern "C" struct CompiledResourceDirectoryEntry test_resource_index[];
extern "C" int test_resource_index_size;

#include "server.h"

#include "cStyleExample.h"

using namespace corecvs;
using namespace std;

void on_resource_image_request(struct evhttp_request *req, void *arg)
{
    evbuffer *evb = evbuffer_new(); // Creating a response buffer
    if (!evb) return;               // No pointer returned

    // Add image to buffer
    evbuffer_add(evb, res__someOtherImages_jojo0_jpg, res__someOtherImages_jojo0_jpg_size);
    evhttp_add_header(req->output_headers, "Content-Type", "image/jpg");

    evhttp_send_reply(req, HTTP_OK, "OK", evb);
    evbuffer_free(evb);
}

[[noreturn]] void server_loop(LibEventServer * server) {
    int count = 0;
    while (true) {
        usleep(10);
        server->process_requests();
        count++;
    }
}

class RefDAO : public ReflectionModuleDAO {
public:
    RgbColorParameters params;
    LockableObject *lockable;

    RefDAO() {
        lockable = new LockableObject(&params);
    }

    std::vector<std::string>  getReflectionNames() override
    {
        return std::vector<std::string>({"example1", "example2"});
    };

    corecvs::LockableObject *getReflectionObject(std::string name) override
    {
        if ( name == "example1" )
        {
            return lockable;
        }
        return nullptr;
    }
};


class ImageDAO : public ImageListModuleDAO {
public:
    RGB24Buffer *buffer = nullptr;

    ImageDAO()
    {
        buffer = new RGB24Buffer(100, 100);
        buffer->checkerBoard(10, RGBColor::Cyan(), RGBColor::Yellow());
    }

    std::vector<std::string> getImageNames() override
    {
        return std::vector<std::string>({"example1"});
    }

    MetaImage getImage(std::string name) override
    {
        if ( name == "example1" )
        {
            SYNC_PRINT(("ImageDAO::getImage(): will return [%d x %d]\n", buffer->w, buffer->h));
            return MetaImage(std::make_shared<RGB24Buffer>(buffer));
        }
        return  MetaImage();
    }
};

class StatisticsDAO : public StatisticsModuleDAO
{
    corecvs::BaseTimeStatisticsCollector collector;

public:
    StatisticsDAO()
    {
        Statistics stats;
        stats.setValue("value1", 100  );
        stats.setTime ("value2", 10000);
        collector.addStatistics(stats);
    }

    corecvs::BaseTimeStatisticsCollector getCollector() override
    {
        return  collector;
    }
};

class GraphDAO : public GraphModuleDAO
{
    GraphData data;

public:
    GraphDAO() {
        for (int i = 0; i < 100; i++) {
            data.addGraphPoint("test", 10 * sin(i / 10.0), true);
        }
    }

    void lockGraphData() override { };
    void unlockGraphData() override { };

    GraphData *getGraphData() override
    {
        return &data;
    }

};

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
    std::thread *mThread = nullptr;
    SYNC_PRINT(("Starting web server 1...\n"));
    auto *server = new LibEventServer(8040, "0.0.0.0", 1);

    /* Start first basic webserver */
    {
        server->setup();

        server->override_callback("/test_img.bmp"  , on_resource_image_request);
        server->set_callback("/"                   , on_get_index , server);

        server->set_callback("/test"               , on_get_test_data, server );
        server->set_callback("/test_img.bmp"       , on_get_test_image, server);

        server->set_callback("/image_request"      , on_image_request, server);
        server->set_callback("/stats_request"      , on_get_stats_request, server);

#ifdef poll_in_C_style
        server->set_callback("/long_poll"          , on_long_pull_request_subscribe, server->poll.get());
        server->set_callback("/long_poll_announce" , on_long_poll_request_announce, server->poll.get());

        server->poll->addEvent("CLIENT_REQUEST", on_get_test_image);
#endif

        server->set_callback("/change_stat_request", on_change_stats_request, server);
        server->set_default_callback(on_other_requests);

        /* Start server thread */
        mThread = new std::thread(server_loop, server);
    }
#endif

#define MODULAR
#ifdef MODULAR
    /* Start modular webserver */
    auto modularServer = new HttpServer(8041, "0.0.0.0", 2);

    auto reflectionModule = new ReflectionListModule;
    reflectionModule->mReflectionsDAO = new RefDAO;
    reflectionModule->setPrefix("/R/");
    modularServer->addModule(reflectionModule);

    auto imageModule = new ImageListModule;
    imageModule->mImages = new ImageDAO;
    imageModule->setPrefix("/I/");
    modularServer->addModule(imageModule);

    auto statsModule = new StatisticsListModule;
    statsModule->mStatisticsDAO = new StatisticsDAO;
    statsModule->setPrefix("/S/");
    modularServer->addModule(statsModule);

    auto graphModule = new GraphModule;
    graphModule->mGraphData = new GraphDAO;
    graphModule->setPrefix("/G/");
    modularServer->addModule(graphModule);

    auto packModule = new ResourcePackModule(test_resource_index, test_resource_index_size);
    packModule->setPrefix("/");
    modularServer->addModule(packModule);

    modularServer->addEvent("cameraImage", imageModule);

    modularServer->setup();
    modularServer->start();



#endif

    /* Run some background task */
    int count = 0;
    while (true) {
        char animation[4] = {'|', '/', '-', '\\' };
        SYNC_PRINT(("\r%c", animation[count % 4]));
        usleep(1000000);
        count++;

        if (count % 100) {
            // modularServer->poll->announce("cameraImage");
        }
    }

    SYNC_PRINT(("Exiting web server...\n"));
    return 0;
}

