#include <iostream>
#include "bmpLoader.h"
#include "rgb24Buffer.h"
#include "libjpegFileReader.h"
#include "libpngFileReader.h"

using namespace std;
using namespace corecvs;

int main(int argc, char *argv[])
{
#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
#endif

    if (argc != 2)
       return 1;

    RGB24Buffer *out = BufferFactory::getInstance()->loadRGB24Bitmap(argv[1]);
    BMPLoader().save("out.bmp", out);
    delete_safe(out);
    return 0;
}

