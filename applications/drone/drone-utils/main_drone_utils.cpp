#include "core/utils/utils.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/buffers/bufferFactory.h"
#include "core/filesystem/folderScanner.h"
#include "core/reflection/commandLineSetter.h"

#include <wrappers/jsonmodern/jsonModernReader.h>


#include <world/simulationWorld.h>

#include <reflection/jsonPrinter.h>

using namespace corecvs;

//extern char _binary_foo_bar_start[];
//extern char _binary_foo_bar_end[];

int main(int argc, char **argv)
{
    CommandLineSetter s(argc, argv);

    if (s.hasOption("dumpWorld"))
    {
        SYNC_PRINT(("dumpWorld called\n"));
        SimulationWorld world;

        world.name = "MyDummyWorld";
        world.meshes.resize(2);
        world.meshes[0].meshName = "models/mesh1.obj";
        world.meshes[0].transform = Matrix44::Identity();
        world.meshes[1].meshName = "models/mesh1.obj";
        world.meshes[1].transform = Matrix44::RotationX(degToRad(5));

        {
            JSONPrinter printer;
            world.accept<JSONPrinter>(printer);

        }


        return 0;
    }

    if (s.hasOption("loadWorld"))
    {

        std::string name = s.getString("input");
        SYNC_PRINT(("loadWorld called\n"));


        SimulationWorld world;
        {
            JSONModernReader reader(name);
            world.accept<JSONModernReader>(reader);
        }
        {
            JSONPrinter printer;
            world.accept<JSONPrinter>(printer);

        }


        return 0;
    }

    SYNC_PRINT(("Usage:\n"));
    SYNC_PRINT(("\n"));
    SYNC_PRINT((" - ./bin/drone-utils --dumpWorld\n"));
    SYNC_PRINT((" - ./bin/drone-utils --loadWorld --input=\"my.json\"\n"));




    return 0;
}


