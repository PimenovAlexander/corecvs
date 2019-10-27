#include <iostream>
#include <fstream>

#include <core/cameracalibration/projection/pinholeCameraIntrinsics.h>

#include <core/geometry/raytrace/materialExamples.h>

#include <core/reflection/jsonPrinter.h>

#include <core/buffers/bufferFactory.h>
#include "core/reflection/commandLineSetter.h"


#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace corecvs;

void printUsage()
{
    SYNC_PRINT(("--caps "));
}

int main(int argc, char **argv)
{
#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    LibjpegFileSaver::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif   

    CommandLineSetter line(argc, argv);

    if (line.hasOption("caps")) {
        BufferFactory::getInstance()->printCaps();
        return 0;
    }

    Vector2dd size(2000, 2000);

    //std::unique_ptr<CameraProjection> camera(new PinholeCameraIntrinsics(Vector2dd(2000, 2000), degToRad(170)));
    std::unique_ptr<CameraProjection> camera(new StereographicProjection(size / 2, 450, size));

    if (line.hasOption("dumpConfig"))
    {
        std::unique_ptr<CameraProjection> prj1(new PinholeCameraIntrinsics);
        std::unique_ptr<CameraProjection> prj2(new OmnidirectionalProjection);
        std::unique_ptr<CameraProjection> prj3(new EquidistantProjection);
        std::unique_ptr<CameraProjection> prj4(new StereographicProjection);
        std::unique_ptr<CameraProjection> prj5(new EquisolidAngleProjection);

        {
            JSONPrinter printer;
            ProjectionFactory factory(prj1);
            printer.visit(factory, "intrinsics");
            cout << endl;
        }
        {
            JSONPrinter printer;
            ProjectionFactory factory(prj2);
            printer.visit(factory, "intrinsics");
            cout << endl;
        }
        {
            JSONPrinter printer;
            ProjectionFactory factory(prj3);
            printer.visit(factory, "intrinsics");
            cout << endl;
        }
        {
            JSONPrinter printer;
            ProjectionFactory factory(prj4);
            printer.visit(factory, "intrinsics");
            cout << endl;
        }
        {
            JSONPrinter printer;
            ProjectionFactory factory(prj5);
            printer.visit(factory, "intrinsics");
            cout << endl;
        }

        return 0;
    }

    string cubePartNames[RaytraceableCubemap::NUMBER_OF_PARTS];
    if (line.hasOption("carla"))
    {
        std::string basename = line.getString("carla");
        std::string file = line.getString("file", "000_000000.png");

        cubePartNames[RaytraceableCubemap::FRONT ] = basename + PATH_SEPARATOR + "rgb1-front"  + PATH_SEPARATOR + file;
        cubePartNames[RaytraceableCubemap::BACK  ] = basename + PATH_SEPARATOR + "rgb1-back"   + PATH_SEPARATOR + file;
        cubePartNames[RaytraceableCubemap::LEFT  ] = basename + PATH_SEPARATOR + "rgb1-left"   + PATH_SEPARATOR + file;
        cubePartNames[RaytraceableCubemap::RIGHT ] = basename + PATH_SEPARATOR + "rgb1-right"  + PATH_SEPARATOR + file;
        cubePartNames[RaytraceableCubemap::TOP   ] = basename + PATH_SEPARATOR + "rgb1-top"    + PATH_SEPARATOR + file;
        cubePartNames[RaytraceableCubemap::BOTTOM] = basename + PATH_SEPARATOR + "rgb1-bottom" + PATH_SEPARATOR + file;
    }

    RGB24Buffer *cubeParts[RaytraceableCubemap::NUMBER_OF_PARTS];
    for (int i = 0; i < RaytraceableCubemap::NUMBER_OF_PARTS; i++)
    {
        cubeParts[i] = BufferFactory::getInstance()->loadRGB24Bitmap(cubePartNames[i]);
        if (cubeParts[i] == NULL)
        {
            SYNC_PRINT(("Can't load <%s>\n", cubePartNames[i].c_str()));
        }
    }

    RaytraceableCubemap6Images cubemap(cubeParts);

    RGB24Buffer *output = new RGB24Buffer(camera->h(), camera->w());

    parallelable_for(0, (int)output->h, [&](const corecvs::BlockedRange<int> &r) {
        for (int i = r.begin(); i != r.end(); i++)
        {
            for (int j = 0; j < output->w; j++)
            {
                Vector2dd pixel(j, i);
                RaytraceRenderer rr;
                Vector3dd d = camera->reverse(pixel);
                RayIntersection ray;
                ray.ray.a = d;
                cubemap.getColor(ray, rr);
                output->element(i, j) = RGBColor::FromDouble(ray.ownColor);
            }
        }
    });

    std::string outputName = line.getString("output", "reproj.bmp");
    BufferFactory::getInstance()->saveRGB24Bitmap(output, outputName);

    return 0;
}

