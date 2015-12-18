/*
 * decoupleYUYV.cpp
 *
 *  Created on: Aug 3, 2012
 *      Author: alexander
 */

#include "decoupleYUYV.h"

namespace corecvs
{


void DecoupleYUYV::decoupleAnaglythSSE(unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCaptureInterface::FramePair &result, bool withSSE)
{
    result.allocBuffers(formatH, formatW);

    for (unsigned i = 0; i < formatH; i++)
    {
        unsigned j = 0;
#ifdef WITH_SSE
        if (withSSE)
        for (; j < formatW; j += (SSEReader8BBBB_DDDD::BYTE_STEP / 2))
        {
            FixedVector<Int16x8,4> r = SSEReader8BBBB_DDDD::read((uint32_t *)ptr);

            Int16x8 cy1 = r[0] - Int16x8((uint16_t) 16);
            Int16x8 cu  = r[1] - Int16x8((uint16_t)128);
            Int16x8 cy2 = r[2] - Int16x8((uint16_t) 16);
            Int16x8 cv  = r[3] - Int16x8((uint16_t)128);

            Int16x8 con0  ((int16_t)0);
            Int16x8 con12bit((uint16_t)0xFFF);

            /* coefficients */

            Int16x8 con100((uint16_t)(100 / 2));
            Int16x8 con128((uint16_t)(128 / 2));
            Int16x8 con208((uint16_t)(208 / 2));
            Int16x8 con298((uint16_t)(298 / 2));
            Int16x8 con516((uint16_t)(516 / 2));
            Int16x8 con409((uint16_t)(409 / 2));

            Int16x8 r1 = shiftLogical(con298 * cy1               + con409 * cv + con128, 3);
            Int16x8 g1 = shiftLogical(con298 * cy1 - con100 * cu - con208 * cv + con128, 3);
            Int16x8 b1 = shiftLogical(con298 * cy1 + con516 * cu               + con128, 3);

            Int16x8 r2 = shiftLogical(con298 * cy2               + con409 * cv + con128, 3);
            Int16x8 g2 = shiftLogical(con298 * cy2 - con100 * cu - con208 * cv + con128, 3);
            Int16x8 b2 = shiftLogical(con298 * cy2 + con516 * cu               + con128, 3);

            /* Now we have r1 and r2 vector, let's interleave them */
            FixedVector<Int16x8,2> red;
            red[0] = r1;
            red[1] = r2;

            FixedVector<Int16x8,2> green;
            green[0] = shiftLogical(g1 + b1, 1);
            green[1] = shiftLogical(g2 + b2, 1);

            for (int k = 0; k < 2; k++)
            {
                red  [k] = SSEMath::selector(  red[k] > con12bit, con12bit,   red[k]);
                green[k] = SSEMath::selector(green[k] > con12bit, con12bit, green[k]);

                red  [k] = SSEMath::selector(  red[k] > con0,   red[k], con0);
                green[k] = SSEMath::selector(green[k] > con0, green[k], con0);
            }

            SSEReader8DD_DD::write(red, (uint32_t *)&result.bufferLeft ->element(i,j));
            SSEReader8DD_DD::write(green, (uint32_t *)&result.bufferRight->element(i,j));
            ptr += SSEReader8BBBB_DDDD::BYTE_STEP;
        }
#endif

        for (; j < formatW; j += 2)
        {
            int y1 = ptr[0];
            int u  = ptr[1];
            int y2 = ptr[2];
            int v  = ptr[3];

            int cy1 = y1 -  16;
            int cu  = u  - 128;
            int cy2 = y2 -  16;
            int cv  = v  - 128;

            int r1 = ((298 * cy1            + 409 * cv + 128) >> 8);
            int g1 = ((298 * cy1 - 100 * cu - 208 * cv + 128) >> 8);
            int b1 = ((298 * cy1 + 516 * cu            + 128) >> 8);

            if (r1 > 255) r1 = 255;  if (r1 < 0) r1 = 0;
            if (g1 > 255) g1 = 255;  if (g1 < 0) g1 = 0;
            if (b1 > 255) b1 = 255;  if (b1 < 0) b1 = 0;

            int r2 = ((298 * cy2            + 409 * cv + 128) >> 8);
            int g2 = ((298 * cy2 - 100 * cu - 208 * cv + 128) >> 8);
            int b2 = ((298 * cy2 + 516 * cu            + 128) >> 8);

            if (r2 > 255) r2 = 255;  if (r2 < 0) r2 = 0;
            if (g2 > 255) g2 = 255;  if (g2 < 0) g2 = 0;
            if (b2 > 255) b2 = 255;  if (b2 < 0) b2 = 0;

            result.bufferLeft ->element(i,j    ) = (r1 << 4);
            result.bufferLeft ->element(i,j + 1) = (r2 << 4);

            result.bufferRight->element(i,j    ) = ((b1 + g1) << 3);
            result.bufferRight->element(i,j + 1) = ((b2 + g2) << 3);

            ptr += 4;
        }
    }

}

/**
 * @brief DecoupleYUYV::decoupleAnaglythSyncCam1
 *
 * Synccam matrix produces a Bayer Image of 12 valuable bits in every 16 bits
 * <pre>
 * 1.                               2.
 *  G R G R G R G   |   G R G R G R G
 *  B G B G B G B   |   B G B G B G B
 *  G R G R G R G   |   G R G R G R G
 *  B G B G B G B   |   B G B G B G B
 *       (MT9P031 Data Sheet, Rev. G, Figure 5, page 10)
 *
 *     Driver so far produces (R,G,B are bytes)
 *
 * G 0x80 R 0x80 G 0x80 R 0x80    |  G 0x80 R 0x80 G 0x80 R 0x80
 * B 0x80 G 0x80 B 0x80 G 0x80    |  B 0x80 G 0x80 B 0x80 G 0x80
 * G 0x80 R 0x80 G 0x80 R 0x80    |  G 0x80 R 0x80 G 0x80 R 0x80
 * B 0x80 G 0x80 B 0x80 G 0x80    |  B 0x80 G 0x80 B 0x80 G 0x80
 *  </pre>
 *
 *  As input to this functions formatH - is equal to matrix height, formatW is a two times matrix width -
 *     doubled beacause of two images side by side
 *  The output will be two images with height and width of formatH, formatW
 *
 *
 *
 * @param formatH
 * @param formatW
 * @param ptr
 * @param result
 **/
void DecoupleYUYV::decoupleAnaglythSyncCam1(unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCaptureInterface::FramePair &result)
{
    //FixMe: use result.allocBuffersRGB(formatH, formatW / 2); ?
    //
    result.rgbBufferLeft  = new RGB24Buffer(formatH, formatW / 2, false);
    result.rgbBufferRight = new RGB24Buffer(formatH, formatW / 2, false);

    for (unsigned i = 0; i + 1 < formatH; i += 2)
    {
        uint8_t *line0 = ptr   + 2 * formatW * i;
        uint8_t *line1 = line0 + 2 * formatW;

        RGBColor *outLine0Left = &result.rgbBufferLeft->element(    i, 0);
        RGBColor *outLine1Left = &result.rgbBufferLeft->element(i + 1, 0);

        for (unsigned j = 0; j < formatW / 4; j++)
        {
            uint8_t g1 = line0[0];
            uint8_t r  = line0[2];
            uint8_t b  = line1[0];
            uint8_t g2 = line1[2];

            RGBColor color (r, (g1 + g2) / 2, b);
            outLine0Left[0] = color;
            outLine0Left[1] = color;
            outLine1Left[0] = color;
            outLine1Left[1] = color;

            line0 += 4;
            line1 += 4;
            outLine0Left += 2;
            outLine1Left += 2;
        }

        RGBColor *outLine0Right = &result.rgbBufferRight->element(    i, 0);
        RGBColor *outLine1Right = &result.rgbBufferRight->element(i + 1, 0);

        for (unsigned j = 0; j < formatW / 4; j++)
        {
            uint8_t g1 = line0[0];
            uint8_t r  = line0[2];
            uint8_t b  = line1[0];
            uint8_t g2 = line1[2];

            RGBColor color (r, (g1 + g2) / 2, b);
            outLine0Right[0] = color;
            outLine0Right[1] = color;
            outLine1Right[0] = color;
            outLine1Right[1] = color;

            line0 += 4;
            line1 += 4;
            outLine0Right += 2;
            outLine1Right += 2;
        }
    }

    result.bufferLeft  = result.rgbBufferLeft ->toG12Buffer();
    result.bufferRight = result.rgbBufferRight->toG12Buffer();
}


void DecoupleYUYV::decoupleSideBySide(unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCaptureInterface::FramePair &result)
{
    result.allocBuffers(formatH, formatW / 2);

    for (unsigned i = 0; i < formatH; i++)
    {
        for (unsigned j = 0; j < formatW / 2; j++)
        {

            result.bufferLeft ->element(i,j) = (ptr[0] << 4);
            ptr += 2;
        }
        for (unsigned j = 0; j < formatW / 2; j++)
        {
            result.bufferRight->element(i,j) = (ptr[0] << 4);
            ptr += 2;
        }
    }
}

void DecoupleYUYV::decoupleAnaglythUnrolled(unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCaptureInterface::FramePair &result)
{
    result.allocBuffers(formatH / 2, formatW / 2, true); // shouldInit

    for (unsigned i = 0; i < formatH / 2; i++)
    {
        uint8_t *ptr1 = ptr  + (formatW * 2) * 2 * i;
        uint8_t *ptr2 = ptr1 + (formatW * 2);

        for (unsigned j = 0; j < formatW / 2; j++)
        {
            // line 1
            int y = (ptr1[0] + ptr1[2]) / 2;
            int u = ptr1[1];
            int v = ptr1[3];

            int C = y -  16;
            int D = u  - 128;
            int E = v  - 128;

            int r = ((298 * C           + 409 * E + 128) >> 8);
            int g = ((298 * C - 100 * D - 208 * E + 128) >> 8);
            int b = ((298 * C + 516 * D           + 128) >> 8);

            if (r > 255) r = 255;  if (r < 0) r = 0;
            if (g > 255) g = 255;  if (g < 0) g = 0;
            if (b > 255) b = 255;  if (b < 0) b = 0;

            result.bufferLeft ->element(i,j) = (r << 4);
            result.bufferRight->element(i,j) = ((b + g) << 3);

            // line 2
            y = (ptr2[0] + ptr2[2]) / 2;
            u = ptr2[1];
            v = ptr2[3];

            C = y -  16;
            D = u  - 128;
            E = v  - 128;

            r = ((298 * C           + 409 * E + 128) >> 8);
            g = ((298 * C - 100 * D - 208 * E + 128) >> 8);
            b = ((298 * C + 516 * D           + 128) >> 8);

            if (r > 255) r = 255;  if (r < 0) r = 0;
            if (g > 255) g = 255;  if (g < 0) g = 0;
            if (b > 255) b = 255;  if (b < 0) b = 0;

            result.bufferLeft ->element(i,j) += (r << 4);
            result.bufferRight->element(i,j) += ((b + g) << 3);

            result.bufferLeft ->element(i,j) /= 2;
            result.bufferRight->element(i,j) /= 2;

            ptr1 += 4;
            ptr2 += 4;
        }
    }

}


void DecoupleYUYV::decouple(unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCouplingType coupling, ImageCaptureInterface::FramePair &result)
{
    switch (coupling) {
        case ANAGLYPH_RC:
            decoupleAnaglythSSE(formatH, formatW, ptr, result, false);
            break;
        case ANAGLYPH_RC_FAST:
            decoupleAnaglythSSE(formatH, formatW, ptr, result, true);
            break;
        case SIDEBYSIDE_STEREO:
            decoupleSideBySide(formatH, formatW, ptr, result);
            break;
        case SIDEBYSIDE_SYNCCAM_1:
            decoupleAnaglythSyncCam1(formatH, formatW, ptr, result);
            break;
        default:
            decoupleAnaglythUnrolled(formatH, formatW, ptr, result);
            break;
    }
}

} /* namespace corecvs */
