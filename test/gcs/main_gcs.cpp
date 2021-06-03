#include <fstream>
#include <graphModule.h>
#include <imageListModule.h>
#include <reflectionListModule.h>
#include <resourcePackModule.h>
#include <statisticsListModule.h>
#include "mavlinkmodule.h"
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

extern "C" struct CompiledResourceDirectoryEntry gcs_resource_index[];
extern "C" int gcs_resource_index_size;

#include "server.h"

using namespace corecvs;
using namespace std;

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
        buffer = new RGB24Buffer(1280, 720);
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
        for (int i = 0; i < 10; i++) {
            data.addGraphPoint("yaw", 10 * sin(i / 10.0), true);
            data.addGraphPoint("pitch", 10 * cos(i / 10.0), true);
            data.addGraphPoint("roll", 10 * tan(i / 10.0), true);
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

    /* Start modular GCS webserver */
    auto gcs = new HttpServer(8042, "0.0.0.0", 2);

    auto images = new ImageListModule;
    images->mImages = new ImageDAO;
    images->setPrefix("/I/");
    gcs->addModule(images);

    auto graphs = new GraphModule;
    graphs->mGraphData = new GraphDAO;
    graphs->setPrefix("/G/");
    gcs->addModule(graphs);

    auto mavlink = new MavLinkModule;
    mavlink->setPrefix("/mavlink/");
    gcs->addModule(mavlink);

    auto resources = new ResourcePackModule(gcs_resource_index, gcs_resource_index_size);
    resources->setPrefix("/");
    gcs->addModule(resources);

    gcs->setup();
    gcs->start();

    /* Run some background task */
    int count = 0;
    while (true) {
        char animation[4] = {'|', '/', '-', '\\' };
        SYNC_PRINT(("\r%c", animation[count % 4]));
        usleep(100000);

        auto image = new ImageDAO;
        auto colourBoard1 = new RGBColor((count * 2) % 256, (count * 3 + 100) % 256, (count * 4 + 200) % 256);
        auto colourBoard2 = new RGBColor((count * 2 + 50) % 256, (count * 3 + 150) % 256, (count * 4 + 250) % 256);

        image->buffer->checkerBoard(10,
                                    *colourBoard1,
                                    *colourBoard2);

        images->mImages = image;

        graphs->mGraphData->getGraphData()->addGraphPoint("yaw", 10 * sin((count+99) / 10.0), true);
        graphs->mGraphData->getGraphData()->addGraphPoint("pitch", 10 * cos((count+99) / 10.0), true);
        graphs->mGraphData->getGraphData()->addGraphPoint("roll", 10 * cos((count+249) / 5.0), true);

        count++;

        count = count % 1200;
    }

    SYNC_PRINT(("Exiting web server...\n"));
    return 0;
}

