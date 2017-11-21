/**
 * \file g12Image.cpp
 *
 * \date Jul 22, 2009
 * \author: Alexander Pimenov
 */
//#include <QPainter>

#include "g12Image.h"
//#include "core/utils/preciseTimer.h"

#ifdef WITH_SSE
#include "core/math/sse/sseWrapper.h"
    //using corecvs::Int32x4;
#endif


G12Image::G12Image(G12Buffer *buffer, int newH, int newW) : QImage(newW, newH, QImage::Format_RGB32)
{
    int bpl = bytesPerLine();
    uint8_t *data = bits(), *line = data;

    for (int i = 0; i < newH; ++i, line += bpl)
    {
        for (int j = 0; j < newW; ++j)
        {
            uint32_t value;
            if (i < buffer->h && j < buffer->w)
            {
                uint8_t c = buffer->element(i, j) >> 4;
                value = c | (c << 8) | (c << 16) | (0xFF << 24);
            }
            else
            {
                value = 0xFF000000UL;
            }

            ((uint32_t *)line)[j] = value;
        }
    }
}


G12Image::G12Image(G12Buffer *buffer, bool mirror) : QImage(buffer->w, buffer->h, QImage::Format_RGB32)
{
    int bpl = bytesPerLine();
    uint8_t *data = bits();

//    PreciseTimer time = PreciseTimer::currentTime();

    if (!mirror)
    {
        for (int i = 0; i < buffer->h; i++)
        {
            uint16_t *lineIn  = (uint16_t *)&buffer->element(i, 0);
            uint32_t *lineOut = (uint32_t *)(data + i * bpl);
            int j = 0;
#ifdef WITH_SSE
            Int32x4 topbyte(0xFF << 24);
            for (; j < buffer->w - 8; j += 8)
            {
                Int16x8 input(lineIn);
                input.shiftLogical(4);
                input = input | (input << 8);
                Int32x4 e1 = Int16x8::unpackLower (input, input);
                Int32x4 e2 = Int16x8::unpackHigher(input, input);
                e1 = e1 | topbyte;
                e2 = e2 | topbyte;
                e1.save(lineOut      );
                e2.save(lineOut + 0x4);
                lineIn  += 8;
                lineOut += 8;
            }
#endif
            for (; j < buffer->w; j++)
            {
                uint8_t c = buffer->element(i, j) >> 4;
                uint32_t value = c | (c << 8) | (c << 16) | (0xFF << 24);
                ((uint32_t *)(data + i * bpl))[j] = value;
            }
        }
    }
    else
    {
        uint8_t *line = data;
        for (int i = 0; i < buffer->h; i++, line += bpl)
        {
            for (int j = 0; j < buffer->w; j++)
            {
                uint8_t      c = buffer->element(i, buffer->w - 1 - j) >> 4;
                uint32_t value = c | (c << 8) | (c << 16) | (0xFF << 24);
                ((uint32_t *)line)[j] = value;
            }
        }
    }

//    printf("Delay %" PRIu64 "\n", time.usecsToNow());
}

G12Image::~G12Image()
{}

G8Image::G8Image(G8Buffer *buffer) : QImage (buffer->w, buffer->h, QImage::Format_RGB32)
{
    int bpl = bytesPerLine();
    uint8_t *data = bits();
    //int depth = image.depth;

    for (int i = 0; i < buffer->h; i++)
    {
        uint8_t  *lineIn  = (uint8_t  *)&buffer->element(i, 0);
        uint32_t *lineOut = (uint32_t *)(data + i * bpl);
        int j = 0;
#ifdef WITH_SSE
            Int32x4 topbyte(0xFF << 24);
            for (; j < buffer->w - 16; j += 16)
            {
                Int8x16 input(lineIn);
                Int16x8 in1 = Int8x16::unpackLower (input, input);
                Int16x8 in2 = Int8x16::unpackHigher(input, input);

                Int32x4 A = Int16x8::unpackLower (in1, in1);
                Int32x4 B = Int16x8::unpackHigher(in1, in1);
                Int32x4 C = Int16x8::unpackLower (in2, in2);
                Int32x4 D = Int16x8::unpackHigher(in2, in2);

                A |= topbyte;
                B |= topbyte;
                C |= topbyte;
                D |= topbyte;

                A.save(lineOut      );
                B.save(lineOut + 0x4);
                C.save(lineOut + 0x8);
                D.save(lineOut + 0xC);

                lineIn  += 16;
                lineOut += 16;
            }
#endif
        for (; j < buffer->w; j++)
        {
            uint8_t c = *lineIn;
            uint32_t value = c | (c << 8) | (c << 16) | (0xFF << 24);
            *lineOut = value;
            lineIn++;
            lineOut++;
        }
    }
}

G8Image::~G8Image()
{}


RGB24Image::RGB24Image(RGB24Buffer *buffer, bool mirror) : QImage (buffer->w, buffer->h, QImage::Format_RGB32)
{
    int bpl = bytesPerLine();
    uint8_t *data = bits();

    if (!mirror)
    {
        for (int i = 0; i < buffer->h; i++)
        {
            uint32_t *lineIn  = (uint32_t *)&buffer->element(i, 0);
            uint32_t *lineOut = (uint32_t *)(data + i * bpl);
            int j = 0;
#ifdef WITH_SSE
            Int32x4 mask(0xFF000000);
            for (j = 0; j < buffer->w - 4; j+= 4 )
            {
                Int32x4 data(lineIn);
                data |= mask;
                data.save(lineOut);
                lineIn  += 4;
                lineOut += 4;
            }
#endif
            for (; j < buffer->w; j++)
            {
                ((uint32_t *)(data + i * bpl))[j] = (buffer->element(i, j).color()) | 0xFF000000;
            }
        }
    }
    else
    {
        uint8_t *line = data;
        for (int i = 0; i < buffer->h; i++, line += bpl)
        {
            for (int j = 0; j < buffer->w; j++)
            {
                ((uint32_t *)line)[j] = (buffer->element(i, buffer->w - 1 - j).color()) | 0xFF000000;
            }
        }
    }
}

RGB24Image::~RGB24Image()
{}

QImage *toQImage(G12Buffer *buffer)
{
    return new G12Image(buffer);
}

QImage *toQImage(G8Buffer *buffer)
{
    return new G8Image(buffer);
}

QImage *toQImage(RGB24Buffer *buffer)
{
    return new RGB24Image(buffer);
}

RGB24InterfaceImage::RGB24InterfaceImage(RGB24Buffer *buffer) :
    QImage(
        (uchar *)buffer->data,
        buffer->w,
        buffer->h,
        buffer->stride * sizeof(RGB24Buffer::InternalElementType),
        QImage::Format_RGB32)
{}

RGB24InterfaceImage::~RGB24InterfaceImage()
{}
