#include <iostream>
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace std;
using namespace corecvs;

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

    {
        RGB24Buffer *out = BufferFactory::getInstance()->loadRGB24Bitmap(argv[1]);
        if (out != NULL)
        {
            SYNC_PRINT(("Image loaded [%d x %d]\n", out->w, out->h));
            BMPLoader().save("out.bmp", out);
            delete_safe(out);
            return 0;
        }
    }
    SYNC_PRINT(("Unable to load RGB image. Loading Grayscale\n"));
    {
        G12Buffer *out = BufferFactory::getInstance()->loadG12Bitmap(argv[1]);
        if (out != NULL)
        {
            SYNC_PRINT(("Image loaded [%d x %d]\n", out->w, out->h));
            for (int i = 0; i < 20; i++) {
                for (int j = 0; j < 20; j++)
                {
                    cout << out->element(i,j) << " ";
                }
                cout << endl;
            }


            BMPLoader().save("out.bmp", out);
            delete_safe(out);
            return 0;
        }
    }
    return 2;
}
