#include <iostream>
#include <fstream>

#include <QApplication>
#include <QtCore>

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#include "qtFileLoader.h"


#include "core/cameracalibration/calibrationCamera.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/geometry/mesh3d.h"
#include "core/fileformats/meshLoader.h"
#include "core/fileformats/objLoader.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"

extern void raytrace_scene1( void );
extern void raytrace_scale( void );

extern void raytrace_scene_scanner( void );
extern void raytrace_scene_calibrate( void );

extern void raytrace_scene_speedup( void );

extern void raytrace_scene_pole( void );

extern void raytrace_scene_large( void );
extern void raytrace_scene_tree(void);

extern void raytrace_scene_union_and_blend(void);

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif
    QTRGB24Loader::registerMyself();

    int scene = 5;
    if ( argc > 1) {
        scene = std::stoi(argv[1]);
        SYNC_PRINT(("Scene id %d\n", scene));
    }

    switch (scene) {
        case 0: raytrace_scene_pole(); break;
        case 1: raytrace_scene1(); break;
        case 2: raytrace_scale(); break;
        case 3: raytrace_scene_scanner(); break;
        case 4: raytrace_scene_calibrate(); break;
        case 5:
        default:
                raytrace_scene_speedup(); break;
        case 6:
                raytrace_scene_large(); break;
        case 7:
                raytrace_scene_tree(); break;
        case 8:
                raytrace_scene_union_and_blend(); break;
    }

}

