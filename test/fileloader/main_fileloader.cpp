#include <iostream>

#include "core/reflection/commandLineSetter.h"
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/abstractPainter.h"

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif

#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#ifdef WITH_LIBGIF
#include "libgifFileReader.h"
#endif

using namespace std;
using namespace corecvs;


void selftest() {
#ifdef WITH_LIBPNG
   RGB24Buffer *rgbInput = new RGB24Buffer(100, 100, RGBColor::Yellow());
   AbstractPainter<RGB24Buffer> p (rgbInput);

   p.drawCircle(25,25, 10, RGBColor::Red());
   p.drawCircle(75,25, 10, RGBColor::Green());
   p.drawCircle(25,75, 10, RGBColor::Blue());
   p.drawCircle(75,75, 10, RGBColor::TiffanyBlue());
#endif
}

void selftest1() {
   cout << "selftest1() called" << endl;
#if 0
   {
       cout << "-----------------Save, load and compare 24 bit buffer-------------------------" << endl;
       RGB24Buffer *rgb24Input = new RGB24Buffer(256, 256);
       for (int i = 0; i < 256; i++)
       {
           for (int j = 0; j < 256; j++)
           {
               rgb24Input->element(i,j) = RGBColor(i,j, (i+j) / 2);
           }
       }

       LibpngFileHelper::save("png24.png", rgb24Input, 95, false);
       RGB24Buffer *rgb24Input1 = LibpngFileHelper::load<RGB24Buffer>("png24.png");

       int value = 0;
       RGB24Buffer *diff = RGB24Buffer::diff(rgb24Input, rgb24Input1, &value);
       cout << value << endl;
   }
   {
       cout << "-----------------Save, load and compare 48 bit buffer-------------------------" << endl;
       RGB48Buffer *rgb48Input = new RGB48Buffer(1024, 1024);
       for (int i = 0; i < 1024; i++)
       {
           for (int j = 0; j < 1024; j++)
           {
               rgb48Input->element(i,j) = RGBTColor<uint16_t>(i * 64 , j * 64, (i+j) * 64/ 2 );
           }
       }

       LibpngFileHelper::save("png48.png", rgb48Input, 95, false);
       RGB48Buffer *rgb24Input1 = LibpngFileHelper::load<RGB48Buffer>("png48.png");

       int value = 0;
       RGB48Buffer *diff = RGB48Buffer::diff(rgb48Input, rgb24Input1, &value);
       cout << value << endl;
   }
#endif


}

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
#ifdef WITH_LIBGIF
    LibgifFileReader::registerMyself();
    SYNC_PRINT(("Libgif support on\n"));
#endif


    CommandLineSetter s(argc, argv);
    cout << "FileLoader launched" << endl;

    if (s.hasOption("selftest")) {
        selftest(); return 0;
    }

    if (s.hasOption("testrgb")) {
        selftest1(); return 0;
    }


    if (argc != 2) {
        BufferFactory::printCaps();
        return 1;
    }

    std::string inputName = argv[1];

#if WITH_LIBPNG
    {
        RGB48Buffer *out = LibpngFileReader().loadRGB48(inputName);
        if (out != NULL)
        {
            SYNC_PRINT(("Loaded 16bit png\n"));
            return 0;
        } else {
            SYNC_PRINT(("Failed to load 16bit png\n"));
        }
    }
#endif

    {
        RGB24Buffer *out = BufferFactory::getInstance()->loadRGB24Bitmap(inputName);
        if (out != NULL)
        {
            SYNC_PRINT(("Image loaded [%d x %d]\n", out->w, out->h));
            BMPLoader().save("out.bmp", out);

#if WITH_LIBJPEG
            vector<unsigned char> data;
            LibjpegFileReader().saveJPEG(data, out);
#endif
#if WITH_LIBGIF
            LibgifFileSaver().save(out, "gif.gif");
#endif

            delete_safe(out);
            return 0;
        }
    }

    {
        FloatFlowBuffer *out = BufferFactory::getInstance()->loadFloatFlow(inputName);
        if (out != NULL)
        {
            SYNC_PRINT(("Flow loaded [%d x %d]\n", out->w, out->h));
            RGB24Buffer *buffer = new RGB24Buffer(out->h, out->w);
            buffer->drawFlowBufferHue(out);
            BMPLoader().save("buffer.bmp", buffer);
            delete_safe(out);
            delete_safe(buffer);
            return 0;
        } else {
             SYNC_PRINT(("Unable to load image like flow.\n"));
        }
    }

    SYNC_PRINT(("Unable to load RGB image. Loading Grayscale\n"));
    {
        G12Buffer *out = BufferFactory::getInstance()->loadG12Bitmap(inputName);
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
