#include <iostream>
#include <vector>
#include <core/cameracalibration/calibrationDrawHelpers.h>
#include <core/cameracalibration/cameraModel.h>
#include <core/geometry/mesh3d.h>
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/tinyxml/tinyxml2.h"
#include "core/math/affine.h"
#include "core/fileformats/opencvDataLoader.h"

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace std;
using namespace corecvs;
using namespace tinyxml2;


int main(int argc, char *argv[])
{
#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

    if (argc != 2) {
        BufferFactory::printCaps();
        return 1;
    }


    OpenCVDataLoader::OpenCVCAL intrinsics = OpenCVDataLoader::parseOpenCVOutput(argv[1]);

    Mesh3D mesh;
    mesh.switchColor(true);

    Vector3dd delta = Vector3dd(intrinsics.bH,  intrinsics.bW, 0) / 2 * intrinsics.sqSize;

    for (int i = 0; i < intrinsics.bH; i++)
    {
        for (int j = 0; j < intrinsics.bW; j++)
        {
            Vector3dd s  = Vector3dd(i    , j     , 0 ) * intrinsics.sqSize - delta;
            Vector3dd e  = Vector3dd(i + 1, j + 1 , 0 ) * intrinsics.sqSize - delta;
            Vector3dd p1 = Vector3dd(s.x(), e.y() , 0 );
            Vector3dd p2 = Vector3dd(e.x(), s.y() , 0 );

            if ( (i % 2) == (j % 2)) {
                mesh.setColor(RGBColor::White());
            } else {
                mesh.setColor(RGBColor::Black());
            }
            mesh.addTriangle( e, p1, s);
            mesh.addTriangle( s, p2, e);
        }
    }

#if 1
    for (int p = 0; p < intrinsics.positions.size(); p++)
    {
        mesh.setColor(RGBColor::getPalleteColor(p));
        mesh.mulTransform(intrinsics.positions[p]);
        CalibrationDrawHelpers().drawCamera(mesh, intrinsics.camera, intrinsics.sqSize);
        mesh.popTransform();
    }
#endif

    mesh.dumpInfo();
    mesh.dumpPLY("decoded.ply");

    RGB24Buffer *buf = NULL;
    buf = BufferFactory::getInstance()->loadRGB24Bitmap("in.bmp");

    if (buf == NULL) {
        buf = new RGB24Buffer( intrinsics.H, intrinsics.W);
        buf->checkerBoard(10, RGBColor::White(), RGBColor::Black());
    }



    RGB24Buffer undist( intrinsics.H, intrinsics.W);
    for (int i = 0; i < undist.h; i++)
    {
        for (int j = 0; j < undist.w; j++)
        {
            Vector2dd in(j,i);
            Vector2dd out = intrinsics.camera.distortion.map(in);
            if (buf->isValidCoordBl(out))
            undist.element(i, j) = buf->elementBl(out);
        }
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(undist, "undist.bmp");

    delete_safe(buf);

   return 0;
}
