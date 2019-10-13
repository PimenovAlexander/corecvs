#include <iostream>
#include "core/fileformats/bmpLoader.h"
#include "imageStack.h"
#include "laplacianStacking.h"
#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif

#ifdef WITH_LIBPNG
#include "core/buffers/rgb24/abstractPainter.h"
#include "libpngFileReader.h"
#endif


using namespace std;
using namespace corecvs;

//Usage: ./focus_stack <path to stack> <path to output dir>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        SYNC_PRINT(("Wrong amount of arguments!\n"));
        return 1;
    }

#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

    ImageStack * imageStack = ImageStack::loadStack(argv[1]);
    if (imageStack == nullptr)
    {
        SYNC_PRINT(("Can't load images!\n"));
        return 0;
    }
    LaplacianStacking lapl;
    imageStack->focus_stack(lapl);
    imageStack->saveMergedImage(argv[2]);

    delete_safe(imageStack);

    return 0;
}

