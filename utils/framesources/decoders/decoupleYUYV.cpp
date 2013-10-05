/*
 * decoupleYUYV.cpp
 *
 *  Created on: Aug 3, 2012
 *      Author: alexander
 */

#include "decoupleYUYV.h"

namespace corecvs
{

void DecoupleYUYV::decouple(unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCouplingType coupling, ImageCaptureInterface::FramePair &result)
{
    if (coupling == ANAGLYPH_RC || coupling == ANAGLYPH_RC_FAST)
    {
        result.bufferLeft  = new G12Buffer(formatH, formatW, false);
        result.bufferRight = new G12Buffer(formatH, formatW, false);

        for (unsigned i = 0; i < formatH; i++)
        {
            unsigned j = 0;
#ifdef WITH_SSE
            if (coupling == ANAGLYPH_RC_FAST)
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

            for (; j < formatW; j+=2)
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
    } else if (coupling == SIDEBYSIDE_STEREO){
        result.bufferLeft  = new G12Buffer(formatH, formatW / 2, false);
        result.bufferRight = new G12Buffer(formatH, formatW / 2, false);

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
    } else {
        result.bufferLeft  = new G12Buffer(formatH / 2, formatW / 2, true);
        result.bufferRight = new G12Buffer(formatH / 2, formatW / 2, true);

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


}

} /* namespace corecvs */
