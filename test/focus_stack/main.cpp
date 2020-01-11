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


void printUsage()
{
    SYNC_PRINT(("./bin/focus_stack <path to stack> <path to output dir>\n or ./bin/focus_stack test for testing"));
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        if  (argc != 2) {
            printUsage();
            return 1;
        }
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
    LaplacianStacking stacking;

    imageStack->focus_stack(stacking);
    imageStack->saveMergedImage(argv[2]);

    delete(imageStack);

    return 0;
}
