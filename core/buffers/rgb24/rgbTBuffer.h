/**
* \file    buffers\rgb24\rgb24Buffer.h
*
* Declares the RGB buffer template class.
*
* \date Oct 15, 2015
* \author alexander
* \author pavel.vasilev
*/

#ifndef CRGBTBUFFER_H_
#define CRGBTBUFFER_H_

#include <stdint.h>

#include "global.h"

#include "fixedPointBlMapper.h"
#include "abstractContiniousBuffer.h"
#include "g12Buffer.h"
#include "g8Buffer.h"
#include "flowBuffer.h"
#include "histogram.h"
#include "rectangle.h"
#include "rgbTColor.h"
#include "function.h"
#include "correspondenceList.h"
#include "imageChannel.h"

#include "conic.h"

#include "readers.h"

namespace corecvs {

#define FLAGS_INCLUDE_MARGIN 0x1

template<typename T>
class RGBTBuffer : public AbstractContiniousBuffer<RGBTColor<T>, int32_t>
                 , public FixedPointBlMapper<RGBTBuffer<T>, RGBTBuffer<T>, int32_t, RGBTColor<T>>
                 , public           BlMapper<RGBTBuffer<T>, RGBTBuffer<T>, int32_t, RGBTColor<T>>
{
    typedef AbstractContiniousBuffer<RGBTColor<T>, int32_t> RGBTBufferBase;
public:
    typedef RGBTColor<T> InternalElementType;
    typedef int32_t      InternalIndexType;

public:
    RGBTBuffer(const RGBTBuffer &that) : RGBTBufferBase(that) {}
    RGBTBuffer(RGBTBuffer *that) : RGBTBufferBase(that) {}

    RGBTBuffer(RGBTBuffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        RGBTBufferBase(src, x1, y1, x2, y2) {}

    RGBTBuffer(int32_t h, int32_t w, RGBTColor<T> *data) : RGBTBufferBase(h, w, data) {}
    RGBTBuffer(int32_t h, int32_t w, const RGBTColor<T> &data) : RGBTBufferBase(h, w, data) {}

    RGBTBuffer(int32_t h, int32_t w, bool shouldInit = true) : RGBTBufferBase(h, w, shouldInit) {}
    RGBTBuffer(Vector2d<int32_t> size, bool shouldInit = true) : RGBTBufferBase(size, shouldInit) {}

    /*Helper Constructors form the relative types*/
    RGBTBuffer(G12Buffer *buffer) : RGBTBufferBase(buffer->h, buffer->w, false)
    {
        drawG12Buffer(buffer);
    }

    /*Helper Constructors form the relative types*/
    RGBTBuffer(G8Buffer *buffer) : RGBTBufferBase(buffer->h, buffer->w, false)
    {
        drawG8Buffer(buffer);
    }

    /**
    * This function is used as a hack to interface the C-style g12Buffer
    *
    **/
    RGBTBuffer() {}

    void drawG12Buffer(G12Buffer *src, int32_t y = 0, int32_t x = 0)
    {
        CORE_ASSERT_TRUE((this->h == src->h && this->w == src->w), "Wrong sizes");
        int i, j;
        for (i = 0; i < src->h; i++)
        {
            for (j = 0; j < src->w; j++)
            {
                uint64_t grey = src->element(i, j) >> 4;
                this->element(i + y, j + x) = RGBTColor<T>(grey, grey, grey);
            }
        }
    }

    void drawG8Buffer(G8Buffer *src, int32_t y = 0, int32_t x = 0)
    {
        CORE_ASSERT_TRUE((this->h == src->h && this->w == src->w), "Wrong sizes");
        int i, j;
        for (i = 0; i < src->h; i++)
        {
            for (j = 0; j < src->w; j++)
            {
                uint64_t grey = src->element(i, j);
                this->element(i + y, j + x) = RGBTColor<T>(grey, grey, grey);
            }
        }
    }

    void drawFlowBuffer(FlowBuffer *src, int32_t y = 0, int32_t x = 0)
    {
        int i, j;
        for (i = 0; i < src->h; i++)
        {
            for (j = 0; j < src->w; j++)
            {
                if (!src->isElementKnown(i, j))
                {
                    continue;
                }
                FlowElement vec = src->element(i, j);

#ifdef ASSERTS
                Vector2d32 res(vec);
                res += Vector2d32(j, i);
                if (!this->isValidCoord(res))
                {
                    CORE_ASSERT_FAIL("Overflow in the flow");
                }
#endif

                RGBTColor<T> color = RGBTColor<T>(128 + (vec.x() * 5), 128 + (vec.y() * 5), 0);

                this->element(i + y, j + x) = color;
            }
        }
    }

    void drawFlowBuffer1(FlowBuffer *src, double colorScaler = 20.0, int32_t y = 0, int32_t x = 0)
    {
        int i, j;
        for (i = 0; i < src->h; i++)
        {
            for (j = 0; j < src->w; j++)
            {
                if (!src->isElementKnown(i, j))
                {
                    continue;
                }
                FlowElement vec = src->element(i, j);

                //RGBTColor<T> color = RGBTColor<T>(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);
                RGBTColor<T> color = RGBTColor<T>::rainbow1(!vec / colorScaler);
                drawLineSimple(j + x, i + y, j + vec.x() + x, i + vec.y() + y, color);
            }
        }
    }

    void drawFlowBuffer2(FlowBuffer *src, double colorShift = 0.0, double colorScaler = 20.0, int32_t y = 0, int32_t x = 0)
    {
        int i, j;
        for (i = 0; i < src->h; i++)
        {
            for (j = 0; j < src->w; j++)
            {
                if (!src->isElementKnown(i, j))
                {
                    continue;
                }
                FlowElement vec = src->element(i, j);

                //RGBTColor<T> color = RGBTColor<T>(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);
                RGBTColor<T> color = RGBTColor<T>::rainbow1((!vec - colorShift) / colorScaler);
                this->drawCrosshare3(j + x, i + y, color);
            }
        }
    }

    void drawFlowBuffer3(FlowBuffer *src, double colorScaler = 20.0, int32_t y = 0, int32_t x = 0)
    {
        int i, j;
        for (i = 0; i < src->h; i++)
        {
            for (j = 0; j < src->w; j++)
            {
                if (!src->isElementKnown(i, j))
                {
                    continue;
                }
                FlowElement vec = src->element(i, j);

                //RGBTColor<T> color = RGBTColor<T>(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);
                RGBTColor<T> color = RGBTColor<T>::rainbow1(!vec / colorScaler);
                drawLineSimple(j + x, i + y, j + vec.x() + x, i + vec.y() + y, color);
                this->drawCrosshare3(j + vec.x() + x, i + vec.y() + y, color);
            }
        }
    }

    void drawCorrespondenceList(CorrespondenceList *src, double colorScaler = 20.0, int32_t y = 0, int32_t x = 0)
    {
        if (src == NULL)
            return;

        CorrespondenceList::iterator it;
        for (it = src->begin(); it != src->end(); ++it)
        {
            Correspondence &tmpCorr = (*it);
            Vector2dd from = tmpCorr.start;
            Vector2dd to = tmpCorr.end;
            Vector2dd vec = to - from;

          //RGBTColor<T> color = RGBTColor<T>(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);
            RGBTColor<T> color = RGBTColor<T>::rainbow1(!vec / colorScaler);

            drawLineSimple(from.x() + x, from.y() + y, to.x() + x, to.y() + y, color);
            this->drawCrosshare3(to.x() + x, to.y() + y, color);
        }
    }

    /**
    * This function is used to draw a pixel in the buffer
    *
    * \param y
    * \param x
    * \param color
    **/
    void drawPixel(int x, int y, RGBTColor<T> color)
    {
        if (this->isValidCoord(y, x))
        {
            this->element(y, x) = color;
        }
    }

    /**
    * This function is used to draw a sort of marker over the buffer
    *  Draws a marker at the point having the form of
    *  <pre>
    *  #   #
    *   # #
    *    o
    *   # #
    *  #   #
    *  </pre>
    *
    * \param y
    * \param x
    * \param color
    *
    **/
    void drawCrosshare1(int x, int y, RGBTColor<T> color)
    {
        int d[8][2] = { { 1,1 },{ 2,2 },{ -1,-1 },{ -2,-2 },{ -1,1 },{ -2,2 },{ 1,-1 },{ 2,-2 } };
        this->drawSprite(x, y, color, d, 8);
    }

    /**
    * This function is used to draw a sort of marker over the buffer
    *<pre>
    *   # #
    *  #   #
    * # # # #
    *    o
    * # # # #
    *  #   #
    *   # #
    *  < / pre>
    *
    * \param y
    * \param x
    * \param color
    *
    **/
    void drawCrosshare2(int x, int y, RGBTColor<T> color)
    {
        int d[16][2] = { { 1,1 },{ 2,2 },{ -1,-1 },{ -2,-2 },{ -1,1 },{ -2,2 },{ 1,-1 },{ 2,-2 },
        { 1,3 },{ 3,1 },{ -1,-3 },{ -3,-1 },{ 1,-3 },{ 3,-1 },{ -1,3 },{ -3,1 } };
        this->drawSprite(x, y, color, d, 16);
    }

    /**  Draws a marker at the point having the form of
    *  <pre>
    *   \#\#\#
    *   \#o\#
    *   \#\#\#
    *  </pre>
    * \param y
    * \param x
    * \param color
    */
    void drawCrosshare3(int x, int y, RGBTColor<T> color)
    {
        int d[8][2] = { { 1,1 },{ 1,0 },{ 1,-1 },{ 0,-1 },{ -1,-1 },{ -1,0 },{ -1,1 },{ 0,1 } };
        this->drawSprite(x, y, color, d, 8);
    }

    void drawHistogram1024x512(Histogram *hist, int x, int y, uint16_t flags = FLAGS_INCLUDE_MARGIN)
    {
        int i, j, k;

        /**
        *  TODO: Add asserts
        **/

        int intervalStart = (flags & FLAGS_INCLUDE_MARGIN) ? 0 : 1;
        int intervalEnd = (flags & FLAGS_INCLUDE_MARGIN) ? G12Buffer::BUFFER_MAX_VALUE : G12Buffer::BUFFER_MAX_VALUE - 1;



        unsigned globalmax = 0;
        for (i = intervalStart; i < intervalEnd; i++)
        {
            if (hist->data[i] > globalmax)
                globalmax = hist->data[i];
        }

        unsigned shift = 0;
        while ((1U << shift) < globalmax)
            shift++;


        for (i = 0; i < 1024; i++)
        {
            int max = 0;
            int min = 0xFFFF;
            int sum = 0;
            int count = 4;

            for (j = 0; j < count; j++)
            {
                int val = hist->data[i * count + j];
                sum += val;
                if (val > max) max = val;
                if (val < min) min = val;
            }

            sum /= count;

            DOTRACE(("%d %d %d\n", min, sum, max));

            int minColumn = (min << 9) >> shift;
            int maxColumn = (max << 9) >> shift;
            int sumColumn = (sum << 9) >> shift;


            for (k = 0; k < minColumn; k++)
            {
                this->element(y + 512 - k, i + x) = RGBTColor<T>(0xFF, 0, 0);
            }
            for (; k < sumColumn; k++)
            {
                this->element(y + 512 - k, i + x) = RGBTColor<T>(0, 0xFF, 0);
            }
            for (; k < maxColumn; k++)
            {
                this->element(y + 512 - k, i + x) = RGBTColor<T>(0, 0, 0xFF);
            }
        }
    }

    /**
    * This function implements the Bresenham algorithm.
    * More details on the Bresenham algorithm here - http://en.wikipedia.org/wiki/Bresenham_algorithm
    *
    * NB! This function doesn't perform clipping it just draws nothing
    * You must clip yourself
    */
    void drawLineSimple(int x1, int y1, int x2, int y2, RGBTColor<T> color)
    {
        if (x1 < 0 || y1 < 0 || x1 >= this->w || y1 >= this->h)
            return;
        if (x2 < 0 || y2 < 0 || x2 >= this->w || y2 >= this->h)
            return;

        int dx = (x2 >= x1) ? x2 - x1 : x1 - x2;
        int dy = (y2 >= y1) ? y2 - y1 : y1 - y2;
        int sx = (x2 >= x1) ? 1 : -1;
        int sy = (y2 >= y1) ? 1 : -1;
        int x, y, i;

        if (dy <= dx)
        {
            int d = (dy << 1) - dx;
            int d1 = dy << 1;
            int d2 = (dy - dx) << 1;

            this->element(y1, x1) = color;

            for (x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx)
            {
                if (d > 0)
                {
                    d += d2;
                    y += sy;
                }
                else
                {
                    d += d1;
                }

                this->element(y, x) = color;
            }
        }
        else
        {
            int d = (dx << 1) - dy;
            int d1 = dx << 1;
            int d2 = (dx - dy) << 1;

            this->element(y1, x1) = color;

            for (x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy)
            {
                if (d > 0)
                {
                    d += d2;
                    x += sx;
                }
                else
                {
                    d += d1;
                }

                this->element(y, x) = color;
            }
        }
    }

    void drawLine(int x1, int y1, int x2, int y2, RGBTColor<T> color)
    {
        Rectangle<int> rect;
        rect.corner = Vector2d<int>(0, 0);
        rect.size = Vector2d<int>(this->w - 1, this->h - 1);
        Vector2d32 lineStart(x1, y1);
        Vector2d32 lineEnd(x2, y2);
        rect.clipCohenSutherland(lineStart, lineEnd);
        drawLineSimple(lineStart.x(), lineStart.y(), lineEnd.x(), lineEnd.y(), color);
    }

    void drawHLine(int x1, int y1, int x2, RGBTColor<T> color)
    {
        if (x1 > x2) { int tmp = x1; x1 = x2; x2 = tmp; }
        if (x1 < 0) x1 = 0;
        if (x2 >= this->w) x2 = this->w - 1;

        if (x1 >= this->w || x2 < 0 || y1 < 0 || y1 >= this->h)
            return;

        for (int j = x1; j < x2; j++)
        {
            this->element(y1, j) = color;
        }
    }

    void drawVLine(int x1, int y1, int y2, RGBTColor<T> color)
    {
        if (y1 > y2) { int tmp = y1; y1 = y2; y2 = tmp; }
        if (y1 < 0) y1 = 0;
        if (y2 >= this->h) y2 = this->h - 1;

        if (y1 >= this->h || y2 < 0 || x1 < 0 || x1 >= this->w)
            return;

        for (int i = y1; i < y2; i++)
        {
            this->element(i, x1) = color;
        }
    }

    void drawArc(int x, int y, int rad, RGBTColor<T> color)
    {
        int radsqr = rad * rad;
        int olddx = 0;
        for (int dy = -rad + 1; dy <= 0; dy++)
        {
            int newdx = fround(sqrt((float)(radsqr - (dy - 0.5) * (dy - 0.5))));

            for (int dx = olddx; dx <= newdx; dx++)
            {
                this->setElement(y - dy, x + dx, color);
                this->setElement(y + dy, x + dx, color);
                this->setElement(y - dy, x - dx, color);
                this->setElement(y + dy, x - dx, color);
            }

            olddx = newdx;
        }
    }

    void drawArc1(int x, int y, int rad, RGBTColor<T> color)
    {
        int radsqr = rad * rad;
        int olddx = 0;
        for (int dy = -rad + 1; dy <= 0; dy++)
        {
            int newdx = (int)sqrt((float)(radsqr - (dy - 0.5) * (dy - 0.5)));

            for (int dx = olddx; dx <= newdx; dx++)
            {
                this->setElement(y - dy, x + dx, color);
                this->setElement(y + dy, x + dx, color);
                this->setElement(y - dy, x - dx, color);
                this->setElement(y + dy, x - dx, color);
            }

            olddx = newdx;
        }
    }

    void drawArc(const Circle2d &circle, RGBTColor<T> color)
    {
        drawArc(circle.c.x(), circle.c.y(), circle.r, color);
    }

    /* Some alternatives */
    void drawLine(double x1, double y1, double x2, double y2, RGBTColor<T> color)
    {
        drawLine(fround(x1), fround(y1), fround(x2), fround(y2), color);
    }

    void drawLine(const Vector2dd &v1, const Vector2dd &v2, RGBTColor<T> color)
    {
        drawLine(v1.x(), v1.y(), v2.x(), v2.y(), color);
    }

    /**
    *   Draw Function
    *
    **/
    void drawFunction(
        double zx, double zy,
        double zh, double zw,
        FunctionArgs &f)
    {
        int i, j;
        if (f.inputs < 2)
            return;

        double *in  = new double[f.inputs];
        double *out = new double[f.outputs];
        for (int i = 0; i < f.inputs; i++)
            in[i] = 0.0;

        for (i = 0; i < this->h; i++)
        {
            for (j = 0; j < this->w; j++)
            {
                in[0] = (i * zh / this->h) + zy;
                in[1] = (j * zw / this->w) + zx;
                f(in, out);
                RGBTColor<T> result = RGBTColor<T>::rainbow(out[0]);
                this->element(i, j) = result;
            }
        }

        delete[] in;
        delete[] out;
    }

    void drawIsolines(
        double zx, double zy,
        double zh, double zw,
        double steps,
        FunctionArgs &f)
    {
        int i, j;
        if (f.inputs < 2)
            return;

        AbstractBuffer<double> *values = new AbstractBuffer<double>(this->getSize());

        double *in  = new double[f.inputs];
        double *out = new double[f.outputs];
        for (int i = 0; i < f.inputs; i++)
        {
            in[i] = 0.0;
        }

        for (i = 0; i < this->h; i++)
        {
            for (j = 0; j < this->w; j++)
            {
                in[0] = (i * zh / this->h) + zy;
                in[1] = (j * zw / this->w) + zx;
                f(in, out);
                values->element(i, j) = out[0];
                /* if (j == w / 2) {
                printf("out: [%d %d] %lf\n", i, j, out[0]);
                }*/
            }
        }

        for (i = 1; i < this->h - 1; i++)
        {
            for (j = 1; j < this->w - 1; j++)
            {
                double c = values->element(i, j);
                double r = values->element(i, j + 1);
                double l = values->element(i + 1, j);

                double ls = ((int)(c * steps)) / steps;
                double hs = ((int)(c * steps) + 1) / steps;
                if (r < ls || r > hs ||
                    l < ls || l > hs)
                    this->element(i, j) = RGBTColor<T>::gray(128);
            }
        }

        deletearr_safe(in);
        deletearr_safe(out);
        delete_safe(values);
    }

    void fillWithYUYV(T *yuyv)
    {
        for (int i = 0; i < this->h; i++)
        {
            int j = 0;
#ifdef WITH_SSE
            const int span = SSEReader8BBBBBBBB_DDDDDDDD::BYTE_STEP / sizeof(RGBTColor<T>);
            /* Checking that we have a full span to read */
            for (; j + span <= this->w; j += span)
            {
                FixedVector<Int16x8, 4> r = SSEReader8BBBB_DDDD::read((uint32_t *)yuyv);

                Int16x8 cy1 = r[0] - Int16x8((uint64_t)16);
                Int16x8 cu = r[1] - Int16x8((uint64_t)128);
                Int16x8 cy2 = r[2] - Int16x8((uint64_t)16);
                Int16x8 cv = r[3] - Int16x8((uint64_t)128);

                Int16x8 con0((int16_t)0);
                Int16x8 con255((int16_t)0xFF);

                /* coefficients */

                /* This is a hack to fit into 16bit register */
                Int16x8 con100((uint64_t)(100 / 4));
                Int16x8 con128((uint64_t)(128 / 4));
                Int16x8 con208((uint64_t)(208 / 4));
                Int16x8 con298((uint64_t)(298 / 4));
                Int16x8 con516((uint64_t)(516 / 4));
                Int16x8 con409((uint64_t)(409 / 4));

                FixedVector<Int16x8, 8> result;
                enum
                {
                    B1,
                    G1,
                    R1,
                    ZERO1,
                    B2,
                    G2,
                    R2,
                    ZERO2
                };

                Int16x8 dr = con128 + con409 * cv;
                Int16x8 dg = con128 - con100 * cu - con208 * cv;
                Int16x8 db = con128 + con516 * cu;

                Int16x8 dy1 = con298 * cy1;
                Int16x8 dy2 = con298 * cy2;

                result[R1] = (dy1 + dr) >> 6;
                result[G1] = (dy1 + dg) >> 6;
                result[B1] = (dy1 + db) >> 6;
                result[ZERO1] = Int16x8((int16_t)0);

                result[R2] = (dy2 + dr) >> 6;
                result[G2] = (dy2 + dg) >> 6;
                result[B2] = (dy2 + db) >> 6;
                result[ZERO2] = Int16x8((int16_t)0);

#ifdef USE_NONUNROLLED_LOOP
                /* TODO: Use saturated arithmetics instead probably*/
                for (int k = B1; k < ZERO1; k++)
                {
                    result[k] = SSEMath::selector(result[k] > con255, con255, result[k]);
                    result[k] = SSEMath::selector(result[k] > con0, result[k], con0);
                    int k1 = k + B2;
                    result[k1] = SSEMath::selector(result[k1] > con255, con255, result[k1]);
                    result[k1] = SSEMath::selector(result[k1] > con0, result[k1], con0);
                }
#else
                result[R1] = SSEMath::selector(result[R1] > con255, con255, result[R1]);
                result[R1] = SSEMath::selector(result[R1] > con0, result[R1], con0);
                result[G1] = SSEMath::selector(result[G1] > con255, con255, result[G1]);
                result[G1] = SSEMath::selector(result[G1] > con0, result[G1], con0);
                result[B1] = SSEMath::selector(result[B1] > con255, con255, result[B1]);
                result[B1] = SSEMath::selector(result[B1] > con0, result[B1], con0);

                result[R2] = SSEMath::selector(result[R2] > con255, con255, result[R2]);
                result[R2] = SSEMath::selector(result[R2] > con0, result[R2], con0);
                result[G2] = SSEMath::selector(result[G2] > con255, con255, result[G2]);
                result[G2] = SSEMath::selector(result[G2] > con0, result[G2], con0);
                result[B2] = SSEMath::selector(result[B2] > con255, con255, result[B2]);
                result[B2] = SSEMath::selector(result[B2] > con0, result[B2], con0);
#endif

                SSEReader8BBBBBBBB_DDDDDDDD::write(result, (uint32_t *)&this->element(i, j));
                yuyv += SSEReader8BBBB_DDDD::BYTE_STEP;
            }
#endif

            for (; j + 2 <= this->w; j += 2)
            {
                int y1 = yuyv[0];
                int u = yuyv[1];
                int y2 = yuyv[2];
                int v = yuyv[3];

                int cy1 = y1 - 16;
                int cu = u - 128;
                int cy2 = y2 - 16;
                int cv = v - 128;

                int r1 = ((298 * cy1 + 409 * cv + 128) >> 8);
                int g1 = ((298 * cy1 - 100 * cu - 208 * cv + 128) >> 8);
                int b1 = ((298 * cy1 + 516 * cu + 128) >> 8);

                if (r1 > 255) r1 = 255;  if (r1 < 0) r1 = 0;
                if (g1 > 255) g1 = 255;  if (g1 < 0) g1 = 0;
                if (b1 > 255) b1 = 255;  if (b1 < 0) b1 = 0;

                int r2 = ((298 * cy2 + 409 * cv + 128) >> 8);
                int g2 = ((298 * cy2 - 100 * cu - 208 * cv + 128) >> 8);
                int b2 = ((298 * cy2 + 516 * cu + 128) >> 8);

                if (r2 > 255) r2 = 255;  if (r2 < 0) r2 = 0;
                if (g2 > 255) g2 = 255;  if (g2 < 0) g2 = 0;
                if (b2 > 255) b2 = 255;  if (b2 < 0) b2 = 0;

                this->element(i, j    ) = RGBTColor<T>(r1, g1, b1);
                this->element(i, j + 1) = RGBTColor<T>(r2, g2, b2);
                yuyv += 4;
            }
        }
    }

    void dropValueAndSatuation(void)
    {
        for (int i = 0; i < this->h; i++)
        {
            for (int j = 0; j < this->w; j++)
            {
                this->element(i, j) = RGBTColor<T>::FromHSV(this->element(i, j).hue(), 255, 255);
            }
        }
    }

    void dropValue()
    {
        for (int i = 0; i < this->h; i++)
        {
            for (int j = 0; j < this->w; j++)
            {
                this->element(i, j) = RGBTColor<T>::FromHSV(this->element(i, j).hue(), this->element(i, j).saturation(), 255);
            }
        }
    }

    G12Buffer *toG12Buffer()
    {
        G12Buffer *toReturn = new G12Buffer(this->h, this->w, false);
        for (int i = 0; i < this->h; i++)
        {
            RGBTColor<T> *in = &this->element(i, 0);
            uint16_t *out = &toReturn->element(i, 0);
            int j = 0;
#ifdef WITH_SSE_
            const int inspan = SSEReader8BBBB_DDDD::BYTE_STEP / sizeof(RGBTColor<T>);
            const int outspan = 8;

            Int16x8 conR(11);
            Int16x8 conG(16);
            Int16x8 conB(5);

            for (; j + inspan <= this->w; j += inspan)
            {
                FixedVector<Int16x8, 4> r = SSEReader8BBBB_DDDD::read((uint64_t*)in);
                enum { B1, G1, R1 };

                Int16x8 result = (conR * r[R1] + conG * r[G1] + conB * r[B1]) >> 1;
                result.save(out);
                in += inspan;
                out += outspan;
            }

#endif
            for (; j < this->w; j++)
            {
                uint16_t result = in->luma12();
                if (result > G12Buffer::BUFFER_MAX_VALUE - 1) result = G12Buffer::BUFFER_MAX_VALUE - 1;
                *out = result;
                in++;
                out++;
            }
        }
        return toReturn;
    }

    /* We need to optimize this */
    G8Buffer* getChannel(ImageChannel::ImageChannel channel)
    {
        G8Buffer *result = new G8Buffer(this->getSize(), false);

        for (int i = 0; i < result->h; i++)
        {
            for (int j = 0; j < result->w; j++)
            {
                T pixel = 0;
                switch (channel)
                {
                case ImageChannel::R:
                    pixel = this->element(i, j).r();
                    break;
                case ImageChannel::G:
                    pixel = this->element(i, j).g();
                    break;
                case ImageChannel::B:
                    pixel = this->element(i, j).b();
                    break;
                default:
                case ImageChannel::LUMA:
                    pixel = this->element(i, j).luma();
                    break;
                case ImageChannel::GRAY:
                    pixel = this->element(i, j).brightness();
                    break;
                case ImageChannel::HUE:
                    pixel = ((int)this->element(i, j).hue()) * 255 / 360;
                    break;
                case ImageChannel::SATURATION:
                    pixel = this->element(i, j).saturation();
                    break;
                case ImageChannel::VALUE:
                    pixel = this->element(i, j).value();
                    break;

                }
                result->element(i, j) = pixel;
            }
        }

        return result;
    }

    template<class SelectorPredicate>
    Vector3dd getMeanValue(int x1, int y1, int x2, int y2, const SelectorPredicate &predicate)
    {
        /* Some sanity check. Could use rectangle operations for this */
        if (x1 < 0) x1 = 0;
        if (y1 < 0) y1 = 0;
        if (x2 < 0) x2 = 0;
        if (y2 < 0) y2 = 0;

        if (x1 > this->w) x1 = this->w;
        if (y1 > this->h) y1 = this->h;
        if (x2 > this->w) x2 = this->w;
        if (y2 > this->h) y2 = this->h;

        if (x1 > x2) { int tmp = x1; x1 = x2; x2 = tmp; }
        if (y1 > y2) { int tmp = y1; y1 = y2; y2 = tmp; }

        int count = 0;
        Vector3dd sum = Vector3dd(0.0);

        for (int i = y1; i < y2; i++)
        {
            for (int j = x1; j < x2; j++)
            {
                if (!predicate(i, j))
                {
                    continue;
                }
                count++;
                sum += this->element(i, j).toDouble();

            }
        }

        if (count == 0)
        {
            return Vector3dd(0.0);
        }
        return sum / count;

    }

    virtual ~RGBTBuffer() {}

    static double diffL2(RGBTBuffer *buffer1, RGBTBuffer *buffer2)
    {
        double sum = 0;
        int h = CORE_MIN(buffer1->h, buffer2->w);
        int w = CORE_MIN(buffer1->w, buffer2->w);

        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                sum += (buffer1->element(i, j).toDouble() - buffer2->element(i, j).toDouble()).sumAllElementsSq() / 3.0;
            }
        }
        sum /= (double)h * w;
        return sum;
    }

    void diffBuffer(RGBTBuffer *that, int *diffPtr)
    {
        int sum = 0;
        int h = CORE_MIN(this->h, that->w);
        int w = CORE_MIN(this->w, that->w);

        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < w; j++)
            {
                RGBTColor<T> diff = RGBTColor<T>::diff(this->element(i, j), that->element(i, j));
                sum += diff.brightness();
                this->element(i, j) = diff;
            }
        }
        if (diffPtr != NULL)
        {
            *diffPtr = sum;
        }
    }

    static RGBTBuffer *diff(RGBTBuffer *buffer1, RGBTBuffer *buffer2, int *diff = NULL)
    {
        RGBTBuffer *toReturn = new RGBTBuffer(buffer1);
        toReturn->diffBuffer(buffer2, diff);
        return toReturn;
    }

    class RGBEx : public FixedVectorBase<RGBEx, uint16_t, 4>
    {
    public:
        RGBEx() {}
        RGBEx(const RGBTColor<T> &color)
        {
            this->at(0) = color.r();
            this->at(1) = color.g();
            this->at(2) = color.b();
            this->at(3) = color.a();
        }

        RGBTColor<T> toRGBTColor() const
        {
            return RGBTColor<T>((T)this->at(0), (T)this->at(1), (T)this->at(2), (T)this->at(3));
        }
    };

    class RGBEx32 : public FixedVectorBase<RGBEx32, uint32_t, 3>
    {
    public:
        RGBEx32() {}
        RGBEx32(const RGBTColor<T> &color)
        {
            this->at(0) = color.r();
            this->at(1) = color.g();
            this->at(2) = color.b();
            this->at(3) = color.a();
        }

        RGBTColor<T> toRGBTColor() const
        {
            return RGBTColor<T>((T)this->at(0), (T)this->at(1), (T)this->at(2), (T)this->at(3));
        }
    };

    /* This should be merged with generic elementBl */
    typename RGBTBuffer::InternalElementType elementBlSSE(double y, double x)
    {
#ifndef WITH_SSE
        return elementBlFixed(y, x);
#else
        /* floor() is needed here because of values (-1..0] which will be
        * rounded to 0 and cause error */
        RGBTBuffer::InternalIndexType i = (RGBTBuffer::InternalIndexType)floor(y);
        RGBTBuffer::InternalIndexType j = (RGBTBuffer::InternalIndexType)floor(x);

        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y, x),
            ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
                y, x, this->w, this->h));

        /* Fixed point */
        const int FIXED_SHIFT = 11;
        uint32_t value = (1 << FIXED_SHIFT);

        Int32x4 k1((int32_t)((x - j) * value));
        Int32x4 k2((int32_t)((y - i) * value));
        Int32x4 v(value);

        FixedVector<Int32x4, 2> in1 = SSEReader2BBBB_QQQQ::read((T *)&this->element(i, j));
        FixedVector<Int32x4, 2> in2 = SSEReader2BBBB_QQQQ::read((T *)&this->element(i + 1, j));

        Int32x4 result =
            (in1[0] * (v - k1) + k1 * in1[1]) * (v - k2) +
            (in2[0] * (v - k1) + k1 * in2[1]) *      k2;
        result >>= (FIXED_SHIFT * 2);

        int32_t data[4];
        result.save(data);
        return RGBTColor<T>(data[2], data[1], data[0]);
#endif
    }

    typename RGBTBuffer::InternalElementType elementBlFixed(double y, double x)
    {
        /* floor() is needed here because of values (-1..0] which will be
        * rounded to 0 and cause error */
        RGBTBuffer::InternalIndexType i = (RGBTBuffer::InternalIndexType)floor(y);
        RGBTBuffer::InternalIndexType j = (RGBTBuffer::InternalIndexType)floor(x);

        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y, x),
            ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
                y, x, this->w, this->h));

        /* Fixed point */
        uint32_t value = 255 * 16;

        uint32_t k1 = (x - j) * value;
        uint32_t k2 = (y - i) * value;

        RGBEx32 a = RGBEx32(this->element(i, j));
        RGBEx32 b = RGBEx32(this->element(i, j + 1));
        RGBEx32 c = RGBEx32(this->element(i + 1, j));
        RGBEx32 d = RGBEx32(this->element(i + 1, j + 1));


        RGBEx32 result =
            (a * (value - k1) + k1 * b) * (value - k2) +
            (c * (value - k1) + k1 * d) *          k2;
        result /= (value * value);
        return result.toRGBTColor();
    }

    /* This should be merged with generic elementBl */
    typename RGBTBuffer::InternalElementType elementBlDouble(double y, double x)
    {
        /* floor() is needed here because of values (-1..0] which will be
        * rounded to 0 and cause error */
        RGBTBuffer::InternalIndexType i = (RGBTBuffer::InternalIndexType)floor(y);
        RGBTBuffer::InternalIndexType j = (RGBTBuffer::InternalIndexType)floor(x);

        CORE_ASSERT_TRUE_P(this->isValidCoordBl(y, x),
            ("Invalid coordinate in AbstractContiniousBuffer::elementBl(double y=%lf, double x=%lf) buffer sizes is [%dx%d]",
                y, x, this->w, this->h));

        /* So far use slow version. Generally this sould be done with fixed point */
        double k1 = x - j;
        double k2 = y - i;

        Vector3dd a = this->element(i, j).toDouble();
        Vector3dd b = this->element(i, j + 1).toDouble();
        Vector3dd c = this->element(i + 1, j).toDouble();
        Vector3dd d = this->element(i + 1, j + 1).toDouble();


        Vector3dd result =
            (a * (1 - k1) + k1 * b) * (1 - k2) +
            (c * (1 - k1) + k1 * d) *      k2;
        return RGBTColor<T>::FromDouble(result);
    }

    typename RGBTBuffer::InternalElementType elementBl(double y, double x)
    {
        return elementBlSSE(y, x);
    }

    typename RGBTBuffer::InternalElementType elementBl(Vector2dd &point)
    {
        return elementBlSSE(point.y(), point.x());
    }

    typename RGBTBuffer::InternalElementType elementBlPrecomp(const BilinearMapPoint &point)
    {
        RGBEx a = RGBEx(this->element(point.y, point.x));
        RGBEx b = RGBEx(this->element(point.y, point.x + 1));
        RGBEx c = RGBEx(this->element(point.y + 1, point.x));
        RGBEx d = RGBEx(this->element(point.y + 1, point.x + 1));

        RGBEx result = (a * point.k1 + b * point.k2 + c * point.k3 + d * point.k4) / 255;
        return result.toRGBTColor();
    }

private:
    /* TODO: Special helper class should be used to store sprites after that this function could be made public */

    void drawSprite(int x, int y, RGBTColor<T> color, int d[][2], int pointNum)
    {
        int i;
        for (i = 0; i < pointNum; i++)
        {
            int xn = x + d[i][0];
            int yn = y + d[i][1];
            drawPixel(xn, yn, color);
        }
    }

};

typedef RGBTBuffer<uint16_t> RGB48Buffer;

} // namespace corecvs

#endif
