#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

#ifdef WITH_AVCODEC
#include "avEncoder.h"
#endif

#ifdef WITH_SWSCALE
#include "swScaler.h"

#include <core/buffers/bufferFactory.h>
#endif

using namespace corecvs;

int main(int argc, char **argv)
{
#ifdef WITH_AVCODEC
    AVEncoder encoder;
    RGB24Buffer buffer(100,100);

    AVEncoder::printCaps();

    encoder.startEncoding("out.avi", buffer.h, buffer.w);
    for (int i = 0; i < 250; i++)
    {
        buffer.checkerBoard(i % 99, RGBColor::Yellow(), RGBColor::Brown());
        AbstractPainter<RGB24Buffer> p(&buffer);
        p.drawFormat(0,0, RGBColor::Navy(), 2, "%d", i);
        encoder.addFrame(&buffer);
    }
    encoder.endEncoding();
#endif


#ifdef WITH_SWSCALE
    RGB24Buffer *in = new RGB24Buffer(100,100);
    in = BufferFactory::getInstance()->loadRGB24Bitmap("data/scene1.row3.col2.BMP");
    //in->checkerBoard(10, RGBColor::Green(), RGBColor::Red());
    G8Buffer *inG = in->getChannelG8(ImageChannel::GRAY);

    SWScaler scaler;
    RGB24Buffer *out  = scaler.scale(in , 0.5);
    G8Buffer    *outG = scaler.scale(inG, 0.5);

    BufferFactory::getInstance()->saveRGB24Bitmap(in , "scaled-before.bmp");
    BufferFactory::getInstance()->saveRGB24Bitmap(out, "scaled-after.bmp");

    RGB24Buffer *inGRGB  = new RGB24Buffer(inG);
    RGB24Buffer *outGRGB = new RGB24Buffer(outG);


    BufferFactory::getInstance()->saveRGB24Bitmap(inGRGB , "scaled-g-before.bmp");
    BufferFactory::getInstance()->saveRGB24Bitmap(outGRGB, "scaled-g-after.bmp");

#endif

    SYNC_PRINT(("Done."));
}

