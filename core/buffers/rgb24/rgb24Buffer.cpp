/**
 * \file rgb24Buffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 1, 2010
 * \author alexander
 */
#include "core/utils/global.h"

#include "core/geometry/rectangle.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/hardcodeFont.h"
#include "core/buffers/kernels/fastkernel/readers.h"
#include "core/math/vector/fixedVector.h"
#include "core/buffers/rgb24/bresenhamRasterizer.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "abstractPainter.h"


#undef rad2     // it's defined at some Windows headers

namespace corecvs {

void RGB24Buffer::drawG12Buffer(const G12Buffer *src, int32_t y, int32_t x)
{
    CORE_ASSERT_TRUE((this->h == src->h && this->w == src->w), "Wrong sizes");
    int i,j;
    for (i = 0; i < src->h; i++)
    {
        for (j = 0; j < src->w; j++)
        {
            uint32_t grey = src->element(i,j) >> 4;
            this->element(i + y, j + x) = RGBColor(grey, grey, grey);
        }
    }
}

void RGB24Buffer::drawG8Buffer(const G8Buffer *src, int32_t y, int32_t x)
{
    CORE_ASSERT_TRUE((this->h == src->h && this->w == src->w), "Wrong sizes");
    int i,j;
    for (i = 0; i < src->h; i++)
    {
        for (j = 0; j < src->w; j++)
        {
            uint32_t grey = src->element(i,j);
            this->element(i + y, j + x) = RGBColor(grey, grey, grey);
        }
    }
}

void RGB24Buffer::drawPixel(int x, int y, const RGBColor &color)
{
    if (this->isValidCoord(y, x))
    {
        element(y, x) = color;
    }
}

void RGB24Buffer::drawPixel(double x, double y, const RGBColor &color)
{
    this->drawPixel(fround(x), fround(y), color);
}

void RGB24Buffer::drawLine(int x1, int y1, int x2, int y2, const RGBColor &color)
{
    Rectangle<int> rect;
    rect.corner = Vector2d<int>(0,0);
    rect.size   = Vector2d<int>(w - 1,h - 1);
    Vector2d32 lineStart(x1,y1);
    Vector2d32 lineEnd(x2,y2);
    rect.clipCohenSutherland(lineStart, lineEnd);
    drawLineSimple(lineStart.x(), lineStart.y(),lineEnd.x(), lineEnd.y(), color);
}

void RGB24Buffer::drawLine(double x1, double y1, double x2, double y2, const RGBColor &color)
{
    drawLine(fround(x1), fround(y1), fround(x2), fround(y2), color);
}

void RGB24Buffer::drawLine(float x1, float y1, float x2, float y2, const RGBColor &color)
{
    drawLine(fround(x1), fround(y1), fround(x2), fround(y2), color);
}

void RGB24Buffer::drawLine(const Vector2dd &v1, const Vector2dd &v2, const RGBColor &color)
{
    drawLine(v1.x(), v1.y(), v2.x(), v2.y(), color);
}

void RGB24Buffer::drawLine(const Vector2df &v1, const Vector2df &v2, const RGBColor &color)
{
    drawLine(v1.x(), v1.y(), v2.x(), v2.y(), color);
}


void RGB24Buffer::drawHLine(int x1, int y1, int x2, const RGBColor &color )
{
    if (x1 > x2)  {int tmp = x1; x1 = x2; x2 = tmp;}
    if (x1 <  0) x1 = 0;
    if (x2 >= w) x2 = w - 1;

    if (x1 >= w || x2 < 0 || y1 < 0 || y1 >= h )
        return;

    for (int j = x1; j <= x2; j++)
    {
        this->element(y1, j) = color;
    }
}

void RGB24Buffer::drawVLine(int x1, int y1, int y2, const RGBColor &color)
{
    if (y1 > y2)  {int tmp = y1; y1 = y2; y2 = tmp;}
    if (y1 <  0) y1 = 0;
    if (y2 >= h) y2 = h - 1;

    if (y1 >= h || y2 < 0 || x1 < 0 || x1 >= w )
        return;

    for (int i = y1; i < y2; i++)
    {
        this->element(i, x1) = color;
    }
}

void RGB24Buffer::drawSprite(int x, int y, const RGBColor &color, int d[][2], int pointNum)
{
    int i;
    for (i = 0; i < pointNum; i++ )
    {
        int xn = x + d[i][0];
        int yn = y + d[i][1];
        drawPixel(xn, yn, color);
    }
}

/**
 *  Draws a marker at the point having the form of
 *  <pre>
 *  #   #
 *   # #
 *    o
 *   # #
 *  #   #
 *  </pre>
 *
 **/
void RGB24Buffer::drawCrosshare1 (int x, int y, const RGBColor &color)
{
    int d[8][2] = {{1,1},{2,2},{-1,-1},{-2,-2},{-1,1},{-2,2},{1,-1},{2,-2}};
    this->drawSprite(x, y, color, d, 8);
}

void RGB24Buffer::drawCrosshare1 (const Vector2dd &point, const RGBColor &color)
{
    int d[8][2] = {{1,1},{2,2},{-1,-1},{-2,-2},{-1,1},{-2,2},{1,-1},{2,-2}};
    this->drawSprite(fround(point.x()), fround(point.y()), color, d, 8);
}

/**  Draws a marker at the point having the form of
 *  <pre>
 *   # #
 *  #   #
 * # # # #
 *    o
 * # # # #
 *  #   #
 *   # #
 *  </pre>
 */

void RGB24Buffer::drawCrosshare2 (int x, int y, const RGBColor &color)
{
    int d[16][2] = {{1,1},{2,2},{-1,-1},{-2,-2},{-1,1},{-2,2},{1,-1},{2,-2},
            {1,3},{3,1},{-1,-3},{-3,-1},{1,-3},{3,-1},{-1,3},{-3,1}};
    this->drawSprite(x, y, color, d, 16);
}


void RGB24Buffer::drawCrosshare2 (const Vector2dd &point, const RGBColor &color)
{
    int d[16][2] = {{1,1},{2,2},{-1,-1},{-2,-2},{-1,1},{-2,2},{1,-1},{2,-2},
            {1,3},{3,1},{-1,-3},{-3,-1},{1,-3},{3,-1},{-1,3},{-3,1}};
    this->drawSprite(fround(point.x()), fround(point.y()), color, d, 16);
}

/**  Draws a marker at the point having the form of
 *  <pre>
 *   \#\#\#
 *   \#o\#
 *   \#\#\#
 *  </pre>
 */
void RGB24Buffer::drawCrosshare3 (int x, int y, const RGBColor &color)
{
    int d[8][2] = {{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1}};
    this->drawSprite(x, y, color, d, 8);
}

void RGB24Buffer::drawCrosshare3 (const Vector2dd &point, const RGBColor &color)
{
    int d[8][2] = {{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1}};
    this->drawSprite(fround(point.x()), fround(point.y()), color, d, 8);
}


void RGB24Buffer::drawFlowBuffer(FlowBuffer *src, int32_t y, int32_t x)
{
    int i,j;
    for (i = 0; i < src->h; i++)
    {
        for (j = 0; j < src->w; j++)
        {
            if (!src->isElementKnown(i,j))
            {
                continue;
            }
            FlowElement vec = src->element(i,j);

#if 0
            Vector2d32 res(vec.x(), vec.y());
            res += Vector2d32(j, i);
            if (!this->isValidCoord(res))
            {
                CORE_ASSERT_FAIL("Overflow in the flow");
            }
#endif

            RGBColor color = RGBColor(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);

            this->element(i + y, j + x) = color;
        }
    }
}


void RGB24Buffer::drawFlowBuffer1(FlowBuffer *src, double colorScaler, int32_t y, int32_t x)
{
    int i,j;
    for (i = 0; i < src->h; i++)
    {
        for (j = 0; j < src->w; j++)
        {
            if (!src->isElementKnown(i,j))
            {
                continue;
            }
            FlowElement vec = src->element(i,j);

            //RGBColor color = RGBColor(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);
            RGBColor color = RGBColor::rainbow1(!vec / colorScaler);
            drawLineSimple(j + x, i + y, j + vec.x() + x, i + vec.y() + y, color);
        }
    }
}

void RGB24Buffer::drawFlowBuffer2(FlowBuffer *src, double colorShift, double colorScaler, int32_t y, int32_t x)
{
    int i,j;
    for (i = 0; i < src->h; i++)
    {
        for (j = 0; j < src->w; j++)
        {
            if (!src->isElementKnown(i,j))
            {
                continue;
            }
            FlowElement vec = src->element(i,j);

            //RGBColor color = RGBColor(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);
            RGBColor color = RGBColor::rainbow1((!vec - colorShift) / colorScaler);
            this->drawCrosshare3(j + x, i + y, color);
        }
    }
}


void RGB24Buffer::drawFlowBuffer3(FlowBuffer *src, double colorScaler, int32_t y, int32_t x)
{
    int i,j;
    for (i = 0; i < src->h; i++)
    {
        for (j = 0; j < src->w; j++)
        {
            if (!src->isElementKnown(i,j))
            {
                continue;
            }
            FlowElement vec = src->element(i,j);

            //RGBColor color = RGBColor(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);
            RGBColor color = RGBColor::rainbow1(!vec / colorScaler);
            drawLineSimple(j + x, i + y, j + vec.x() + x, i + vec.y() + y, color);
            this->drawCrosshare3(j + vec.x() + x, i+ vec.y() + y, color);
        }
    }
}

void RGB24Buffer::drawCorrespondenceList(CorrespondenceList *src, double colorScaler, int32_t y, int32_t x)
{
    if (src == NULL) {
        return;
    }

    CorrespondenceList::iterator it;
    for (it = src->begin(); it != src->end(); ++it)
    {
        Correspondence &tmpCorr = (*it);
        Vector2dd from = tmpCorr.start;
        Vector2dd to = tmpCorr.end;
        Vector2dd vec = to - from;
        //RGBColor color = RGBColor(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);
        RGBColor color = RGBColor::rainbow1(!vec / colorScaler);
        drawLineSimple(from.x() + x, from.y() + y, to.x() + x, to.y() + y, color);
        this->drawCrosshare3(to.x() + x, to.y() + y, color);
    }
}


void RGB24Buffer::drawRectangle(int x, int y, int w, int h, const RGBColor &color, int style)
{
    w--;
    h--;

    if (style == 1)
    {
        this->drawCrosshare3(x    , y    , color);
        this->drawCrosshare3(x + w, y    , color);
        this->drawCrosshare3(x    , y + h, color);
        this->drawCrosshare3(x + w, y + h, color);
        return;
    }

    if (style == 0)
    {
        for (int i = 0; i <= h; i++)
        {
            this->setElement(y + i, x    , color);
            this->setElement(y + i, x + w, color);
        }

        for (int j = 1; j <= w; j++)
        {
            this->setElement(y    , x + j, color);
            this->setElement(y + h, x + j, color);
        }
    }

    if (style == 2) {
        for (int i = 0; i <= h; i++)
        {
            RGBColor *line = &element(i + y, x);
            for (int j = 0; j <= w; j++)
            {
                *line = color;
                line++;
            }
        }

    }
}

void RGB24Buffer::drawRectangle(const Rectangle<int32_t> &rect, const RGBColor &color, int style)
{
    drawRectangle(rect.corner.x(), rect.corner.y(), rect.size.x(), rect.size.y(), color, style);
}


void RGB24Buffer::drawRectangle(const Rectangled &rect, const RGBColor &color, int style)
{
    drawRectangle(
        fround(rect.corner.x()),
        fround(rect.corner.y()),
        fround(rect.size.x()),
                fround(rect.size.y()), color, style);
}

void RGB24Buffer::drawTriangle(const Triangle2dd &tri, const RGBColor &color, int style)
{
    if (style == 1)
    {
        for (int i = 0; i < 3; i++) {
           drawCrosshare3(tri.p[i], color);
        }
        return;
    }

    if (style == 0)
    {
        drawLineSimple(fround(tri.p1().x()), fround(tri.p1().y()), fround(tri.p2().x()), fround(tri.p2().y()), color);
        drawLineSimple(fround(tri.p2().x()), fround(tri.p2().y()), fround(tri.p3().x()), fround(tri.p3().y()), color);
        drawLineSimple(fround(tri.p3().x()), fround(tri.p3().y()), fround(tri.p1().x()), fround(tri.p1().y()), color);
        return;
    }

}


#if 0
void RGB24Buffer::rgb24DrawDisplacementBuffer(RGB24Buffer *dst, DisplacementBuffer *src, double step)
{
    int move[8][2] = {
            {1 ,0},  {1 ,1},  {0 ,1},  {-1,1},
            {-1,0},    {-1,-1}, {0 ,-1}, {1 ,-1}
    };
    int i,j;
    for (i = 1; i < src->h - 1; i++)
    {
        for (j = 1; j < src->w - 1; j++)
        {
            vector2Dd current;
            displacementBufferGetDisplacement(src, j, i, &current.x, &current.y);
            uint32_t color = 0xFFFFFF;
            double len = vector2DdLength(&current);
            int m  = (int)(len / step);
            int m2 = (int)(len / (step / 2));
            double limit     =  step * (m + 1);
            double limit_low = (step / 2) * (m2 + 1);

            int k;
            for (k = 0; k < 8; k++)
            {
                vector2Dd check;
                displacementBufferGetDisplacement(src, j + move[k][0]   , i + move[k][1], &check.x, &check.y);

                if (vector2DdLength(&check) > limit_low)
                    color = 0xAFAFAF;
                if (vector2DdLength(&check) > limit)
                    color = 0x0;
            }

            RGB24ELEM(dst, j, i) = color;
        }
    }
}

void RGB24Buffer::rgb24DrawHistogram(RGB24Buffer *dst, Histogram *hist, int x, int y, int h, int w, uint16_t flags)
{
    int i,j,k;

    if (h == 0)
        return;


    int intervalStart = (flags & FLAGS_INCLUDE_MARGIN) ? 0 : 1;
    int intervalEnd   = (flags & FLAGS_INCLUDE_MARGIN) ? BUFFER_MAX_VALUE : BUFFER_MAX_VALUE - 1;
    double px = intervalStart;


    double dx = (double)(intervalEnd - intervalStart) / w;


    uint32_t globalmax = 0;
    for (i = intervalStart; i < intervalEnd; i++)
    {
        if (hist->data[i] > globalmax)
            globalmax = hist->data[i];
    }

    double scale = (double)h / (globalmax + 1);


    for (i = 0; i < w; i++)
    {
        int step = (int)dx + 1 ;
        int max  =  0;
        int min  =  0xFFFF;
        int sum =   0;
        int count;

        for(j = px, count = 0; j < px + step && j < (intervalEnd - 1); j++, count ++)
        {
            int val = hist->data[j];
            sum += val;
            if (val > max) max = val;
            if (val < min) min = val;
        }

        sum /= count;

        DOTRACE(("%d %d %d\n", min, sum, max));

        for (k = 0; k < min * scale ; k++)
        {
            RGB24ELEM(dst, i + x, y + h - k) = 0xFF0000;
        }
        for (; k < sum * scale; k++)
        {
            RGB24ELEM(dst, i + x, y + h - k) = 0x00FF00;
        }
        for (; k < max * scale; k++)
        {
            RGB24ELEM(dst, i + x, y + h - k) = 0x0000FF;
        }

        px += dx;
    }


}
#endif


void RGB24Buffer::drawHistogram1024x512(Histogram *hist, int x, int y, uint16_t flags, int hw, int hh)
{
    int i,k;

    /**
     *  TODO: Add asserts
     **/
    int intervalStart = (flags & FLAGS_INCLUDE_MARGIN) ? hist->getArgumentMin()     : hist->getArgumentMin() + 1;
    int intervalEnd   = (flags & FLAGS_INCLUDE_MARGIN) ? hist->getArgumentMax() - 1 : hist->getArgumentMax() - 2;

    unsigned globalmax = 0;
    for (i = intervalStart; i < intervalEnd; i++)
    {
        if (hist->data[i] > globalmax)
            globalmax = hist->data[i];
    }

    double scale = 0;
    if (globalmax != 0) {
        scale = (double)hh / globalmax;
        SYNC_PRINT(("Globalmax: %d\n", globalmax));
        SYNC_PRINT(("Scale    : %lf\n", scale));
    }

    if (flags & FLAGS_LIMIT_DOWNSCALE) {
        if (scale > 1.0) {
            scale = 1.0;
        }
    }

    double pos = 0;
    double dpos = (double)hist->data.size() / hw;

     SYNC_PRINT(("DPos    : %lf\n", dpos));

    for (i = 0; i < hw; i++, pos += dpos)
    {
        int max  =  0;
        int min  =  std::numeric_limits<int>::max();
        int sum =   0;


        int count = 0;
        for(int p = pos; (p < pos + dpos) && (p < (int)hist->data.size()); p ++, count++)
        {
            int val = hist->data[p];
            sum += val;
            if (val > max) max = val;
            if (val < min) min = val;
        }

        sum /= count;

        DOTRACE(("%d %d %d\n", min, sum, max));

        int minColumn = min * scale;
        int maxColumn = max * scale;
        int sumColumn = sum * scale;


        for (k = 0; k < minColumn ; k++)
        {
            if (this->isValidCoord(y + hh - k,  i + x))
                this->element(y + hh - k,  i + x) = RGBColor(0xFF,0,0);
        }
        for (; k < sumColumn; k++)
        {
            if (this->isValidCoord(y + hh - k,  i + x))
                this->element(y + hh - k, i + x) = RGBColor(0, 0xFF,0);
        }
        for (; k < maxColumn; k++)
        {
            if (this->isValidCoord(y + hh - k,  i + x))
                this->element(y + hh - k, i + x) = RGBColor(0, 0, 0xFF);
        }
    }
}

void RGB24Buffer::drawLineSimple(int x1, int y1, int x2, int y2, const RGBColor &color)
{
    BresenhamRasterizer::drawLineSimple<RGB24Buffer>(*this, x1, y1, x2, y2, color);
}

void RGB24Buffer::drawLineSimpleFancy(int x1, int y1, int x2, int y2, const RGBColor &color)
{
    WuRasterizer::drawLine<RGB24Buffer>(*this, x1, y1, x2, y2, color);
}

// TODO: make this more pretty
void RGB24Buffer::drawArc(int x, int y, int rad, const RGBColor &color)
{
    int rad2 = rad * rad;
    int olddx = 0;
    for (int dy = -rad + 1; dy <= 0; dy++)
    {
        int newdx = fround(sqrt((float)(rad2 - (dy - 0.5) * (dy - 0.5))));

        for (int dx = olddx; dx <= newdx ; dx++)
        {
            this->setElement(y - dy, x + dx, color);
            this->setElement(y + dy, x + dx, color);
            this->setElement(y - dy, x - dx, color);
            this->setElement(y + dy, x - dx, color);
        }

        olddx = newdx;
    }
}

void RGB24Buffer::drawArc(const Circle2d &circle, const RGBColor &color )
{
    drawArc(circle.c.x(), circle.c.y(), circle.r, color);
}


void RGB24Buffer::drawArc1(int x, int y, int rad, const RGBColor &color)
{
    int rad2 = rad * rad;
    int olddx = 0;
    for (int dy = -rad + 1; dy <= 0; dy++)
    {
        int newdx = (int)sqrt((float)(rad2 - (dy - 0.5) * (dy - 0.5)));

        for (int dx = olddx; dx <= newdx ; dx++) {
            this->setElement(y - dy, x + dx, color);
            this->setElement(y + dy, x + dx, color);
            this->setElement(y - dy, x - dx, color);
            this->setElement(y + dy, x - dx, color);
        }

        olddx = newdx;
    }
}


void RGB24Buffer::drawFunction(
        double zx, double zy,
        double zh, double zw,
        FunctionArgs &f)
{
   int i,j;
   if (f.inputs < 2)
       return;

   double *in =  new double[f.inputs];
   double *out = new double[f.outputs];
   for (int i = 0; i < f.inputs; i++)
       in[i] = 0.0;


   for (i = 0; i < h; i++)
   {
       for (j = 0; j < w; j++)
       {
           in[0] = (i * zh / h)  + zy;
           in[1] = (j * zw / w)  + zx;
           f(in, out);
           RGBColor result = RGBColor::rainbow(out[0]);
           element(i,j) = result;
       }
   }

   delete[] in;
   delete[] out;
}


void RGB24Buffer::drawIsolines(
        double zx, double zy,
        double zh, double zw,
        double steps,
        FunctionArgs &f)
{
   int i,j;
   if (f.inputs < 2)
       return;

   AbstractBuffer<double> *values = new AbstractBuffer<double>(this->getSize());

   double *in =  new double[f.inputs ];
   double *out = new double[f.outputs];
   for (int i = 0; i < f.inputs; i++) {
       in[i] = 0.0;
   }

   for (i = 0; i < h; i++)
   {
       for (j = 0; j < w; j++)
       {
           in[0] = (i * zh / h)  + zy;
           in[1] = (j * zw / w)  + zx;
           f(in, out);
           values->element(i,j) = out[0];
          /* if (j == w / 2) {
               printf("out: [%d %d] %lf\n", i, j, out[0]);
           }*/
       }
   }

   for (i = 1; i < h - 1; i++)
   {
       for (j = 1; j < w - 1; j++)
       {
           double c = values->element(i,j);
           double r = values->element(i,j + 1);
           double l = values->element(i + 1,j);

           double ls = ((int) (c * steps)) / steps;
           double hs = ((int) (c * steps) + 1) / steps;
           if (r < ls || r > hs ||
               l < ls || l > hs )
               element(i,j) = RGBColor::gray(128);
       }
   }

   deletearr_safe(in);
   deletearr_safe(out);
   delete_safe(values);
}

template<typename ContinuousType>
void RGB24Buffer::drawContinuousBuffer(const AbstractBuffer<ContinuousType> &in, int style, bool legend)
{
    int mh = CORE_MIN(h, in.h);
    int mw = CORE_MIN(w, in.w);

    ContinuousType min = std::numeric_limits<ContinuousType>::max();
    ContinuousType max = std::numeric_limits<ContinuousType>::lowest();

    if (style != STYLE_ZBUFFER) {
        for (int i = 0; i < mh; i++)
        {
            for (int j = 0; j < mw; j++)
            {
                min = CORE_MIN(min, in.element(i,j));
                max = CORE_MAX(max, in.element(i,j));
            }
        }
    } else {
        for (int i = 0; i < mh; i++)
        {
            for (int j = 0; j < mw; j++)
            {
                min = CORE_MIN(min, in.element(i,j));
                if (in.element(i,j) != std::numeric_limits<ContinuousType>::max()) {
                    max = CORE_MAX(max, in.element(i,j));
                }
            }
        }
    }

//    SYNC_PRINT(("RGB24Buffer::drawDoubleBuffer(): min %lf max %lf\n", min, max));

    if (style == STYLE_RAINBOW)
    {
        for (int i = 0; i < mh; i++)
        {
            for (int j = 0; j < mw; j++)
            {
                element(i, j) = RGBColor::rainbow(lerp(0.0, 1.0, in.element(i,j), min, max));
            }
        }
    }

    if (style == STYLE_GRAY || style == STYLE_LOG)
    {
        for (int i = 0; i < mh; i++)
        {
            for (int j = 0; j < mw; j++)
            {
                element(i, j) = RGBColor::gray(lerpLimit(0.0, 255.0, in.element(i,j), min, max));
            }
        }
    }

    if (style == STYLE_ZBUFFER)
    {
        for (int i = 0; i < mh; i++)
        {
            for (int j = 0; j < mw; j++)
            {
                if (in.element(i,j) != std::numeric_limits<ContinuousType>::max()) {
                    element(i, j) = RGBColor::rainbow(lerp(0.0, 1.0, in.element(i,j), min, max));
                } else {
                    element(i, j) = RGBColor::Black();
                }
            }
        }
    }

    if (legend) {
        AbstractPainter<RGB24Buffer> painter(this);
        painter.drawFormat(10,10, RGBColor::Black(), 1, "Min:%lf", (double)min);
        painter.drawFormat(11,11, RGBColor::White(), 1, "Min:%lf", (double)min);

        painter.drawFormat(20,20, RGBColor::Black(), 1, "Max:%lf", (double)max);
        painter.drawFormat(21,21, RGBColor::White(), 1, "Max:%lf", (double)max);



    }

}

template void RGB24Buffer::drawContinuousBuffer(const AbstractBuffer<double> &in, int style, bool legend);
template void RGB24Buffer::drawContinuousBuffer(const AbstractBuffer<float> &in, int style, bool legend);


void RGB24Buffer::drawDoubleVecBuffer(const AbstractBuffer<Vector2dd> &in)
{
    int mh = std::min(h, in.h);
    int mw = std::min(w, in.w);

    Vector2dd min = Vector2dd(std::numeric_limits<double>::max   ());
    Vector2dd max = Vector2dd(std::numeric_limits<double>::lowest());

    for (int i = 0; i < mh; i++)
    {
        for (int j = 0; j < mw; j++)
        {
            min = min.cwiseMin(in.element(i,j));
            max = min.cwiseMax(in.element(i,j));
        }
    }

    cout << "RGB24Buffer::drawDoubleVecBuffer(): " << min << " " << max << endl;

    for (int i = 0; i < mh; i++)
    {
        for (int j = 0; j < mw; j++)
        {
            Vector3dd c = Vector3dd((in.element(i,j) - min) / (max - min)  * 255.0, 0.0);
            element(i, j) = RGBColor::FromDouble(c);
        }
    }

}
void RGB24Buffer::fillWithYUYV(uint8_t *data)
{
    fillWithYUVFormat(data, false);
}

void RGB24Buffer::fillWithUYVU(uint8_t *data)
{
    fillWithYUVFormat(data, true);
}

void RGB24Buffer::fillWithYUVFormat (uint8_t *yuyv, bool fillAsUYVY)
{
    cint iy1 = fillAsUYVY ? 1 : 0;
    cint iu  = fillAsUYVY ? 0 : 1;
    cint iy2 = fillAsUYVY ? 3 : 2;
    cint iv  = fillAsUYVY ? 2 : 3;

    for (int i = 0; i < h; i++)
    {
        int j = 0;
#ifdef WITH_SSE
        const int span = SSEReader8BBBBBBBB_DDDDDDDD::BYTE_STEP / sizeof(RGBColor);
        /* Checking that we have a full span to read */
        for (; j + span <= w ; j += span)
        {
            FixedVector<Int16x8,4> r = SSEReader8BBBB_DDDD::read((uint32_t *)yuyv);

            Int16x8 cy1 = r[iy1] - Int16x8((uint16_t) 16);
            Int16x8 cu  = r[iu]  - Int16x8((uint16_t)128);
            Int16x8 cy2 = r[iy2] - Int16x8((uint16_t) 16);
            Int16x8 cv  = r[iv]  - Int16x8((uint16_t)128);

            Int16x8 con0  ((int16_t)0);
            Int16x8 con255((int16_t)0xFF);

            /* coefficients */

            /* This is a hack to fit into 16bit register */
            Int16x8 con100((uint16_t)(100 / 4));
            Int16x8 con128((uint16_t)(128 / 4));
            Int16x8 con208((uint16_t)(208 / 4));
            Int16x8 con298((uint16_t)(298 / 4));
            Int16x8 con516((uint16_t)(516 / 4));
            Int16x8 con409((uint16_t)(409 / 4));

            FixedVector<Int16x8, 8> result;
            enum {
                B1,
                G1,
                R1,
                ZERO1,
                B2,
                G2,
                R2,
                ZERO2
            };

            Int16x8 dr = con128               + con409 * cv;
            Int16x8 dg = con128 - con100 * cu - con208 * cv;
            Int16x8 db = con128 + con516 * cu              ;

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

                result[k] = SSEMath::selector(result[k] > con255, con255   , result[k]);
                result[k] = SSEMath::selector(result[k] > con0  , result[k], con0     );
                int k1 = k + B2;
                result[k1] = SSEMath::selector(result[k1] > con255, con255   , result[k1]);
                result[k1] = SSEMath::selector(result[k1] > con0  , result[k1], con0     );
            }
#else
            result[R1] = SSEMath::selector(result[R1] > con255, con255   , result[R1]);
            result[R1] = SSEMath::selector(result[R1] > con0  , result[R1], con0     );
            result[G1] = SSEMath::selector(result[G1] > con255, con255   , result[G1]);
            result[G1] = SSEMath::selector(result[G1] > con0  , result[G1], con0     );
            result[B1] = SSEMath::selector(result[B1] > con255, con255   , result[B1]);
            result[B1] = SSEMath::selector(result[B1] > con0  , result[B1], con0     );

            result[R2] = SSEMath::selector(result[R2] > con255, con255   , result[R2]);
            result[R2] = SSEMath::selector(result[R2] > con0  , result[R2], con0     );
            result[G2] = SSEMath::selector(result[G2] > con255, con255   , result[G2]);
            result[G2] = SSEMath::selector(result[G2] > con0  , result[G2], con0     );
            result[B2] = SSEMath::selector(result[B2] > con255, con255   , result[B2]);
            result[B2] = SSEMath::selector(result[B2] > con0  , result[B2], con0     );
#endif

            SSEReader8BBBBBBBB_DDDDDDDD::write(result, (uint32_t *)&element(i,j));
            yuyv += SSEReader8BBBB_DDDD::BYTE_STEP;
        }
#endif

        for (; j + 2 <= w; j += 2)
        {
            int y1 = yuyv[iy1];
            int u  = yuyv[iu];
            int y2 = yuyv[iy2];
            int v  = yuyv[iv];

            int cy1 = y1 -  16;
            int cu  = u  - 128;
            int cy2 = y2 -  16;
            int cv  = v  - 128;

            int r1 = ((298 * cy1            + 409 * cv + 128) >> 8);
            int g1 = ((298 * cy1 - 100 * cu - 208 * cv + 128) >> 8);
            int b1 = ((298 * cy1 + 516 * cu            + 128) >> 8);

            int r2 = ((298 * cy2            + 409 * cv + 128) >> 8);
            int g2 = ((298 * cy2 - 100 * cu - 208 * cv + 128) >> 8);
            int b2 = ((298 * cy2 + 516 * cu            + 128) >> 8);

            element(i,j)     = RGBColor(clamp(r1, 0, 255), clamp(g1, 0, 255), clamp(b1, 0, 255));
            element(i,j + 1) = RGBColor(clamp(r2, 0, 255), clamp(g2, 0, 255), clamp(b2, 0, 255));
            yuyv += 4;
        }
    }
}

/*void RGB24Buffer::fillWith420P(uint8_t *y, uint8_t *u, uint8_t *v, int ly, int lu, int lv)
{

}*/

void RGB24Buffer::dropValueAndSatuation()
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            element(i,j) = RGBColor::FromHSV(element(i,j).hue(), 255, 255);
        }
    }
}

void RGB24Buffer::dropValue()
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            element(i,j) = RGBColor::FromHSV(element(i,j).hue(), element(i,j).saturation(), 255);
        }
    }
}


G12Buffer *RGB24Buffer::toG12Buffer()
{
    G12Buffer *toReturn = new G12Buffer(h,w, false);
    for (int i = 0; i < h; i++)
    {
        RGBColor *in  = &this->element(i,0);
        uint16_t *out = &toReturn->element(i,0);
        int j = 0;
#ifdef WITH_SSE_
        const int inspan  = SSEReader8BBBB_DDDD::BYTE_STEP / sizeof(RGBColor);
        const int outspan = 8;

        Int16x8 conR(11);
        Int16x8 conG(16);
        Int16x8 conB( 5);

        for (; j + inspan <= w; j += inspan)
        {
            FixedVector<Int16x8,4> r = SSEReader8BBBB_DDDD::read((uint32_t*)in);
            enum {B1, G1, R1};

            Int16x8 result = (conR * r[R1] + conG * r[G1] + conB * r[B1]) >> 1;
            result.save(out);
            in  += inspan;
            out += outspan;
        }

#endif
        for (; j < w; j++)
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
G8Buffer* RGB24Buffer::getChannel(ImageChannel::ImageChannel channel)
{
    G8Buffer *result = new G8Buffer(getSize(), false);

    for (int i = 0; i < result->h; i++)
    {
        for (int j = 0; j < result->w; j++)
        {
            uint8_t pixel = 0;
            switch (channel)
            {
                case ImageChannel::R:
                    pixel = element(i,j).r();
                    break;
                case ImageChannel::G:
                    pixel = element(i,j).g();
                    break;
                case ImageChannel::B:
                    pixel = element(i,j).b();
                    break;
                case ImageChannel::U:
                    pixel = element(i,j).u();
                    break;
                case ImageChannel::V:
                    pixel = element(i,j).v();
                    break;
                case ImageChannel::CR:
                    pixel = element(i,j).cr();
                    break;
                case ImageChannel::CB:
                    pixel = element(i,j).cb();
                    break;
                case ImageChannel::CHROMA:
                    pixel = element(i,j).chroma();
                    break;

                default:
                case ImageChannel::LUMA:
                    pixel = element(i,j).luma();
                    break;
                case ImageChannel::GRAY:
                    pixel = element(i,j).brightness();
                    break;
                case ImageChannel::HUE:
                    pixel = ((int)element(i,j).hue()) * 255 / 360;
                    break;
                case ImageChannel::SATURATION:
                    pixel = element(i,j).saturation();
                    break;
                case ImageChannel::VALUE:
                    pixel = element(i,j).value();
                    break;

            }
            result->element(i,j) = pixel;
        }
    }

    return result;
}

double RGB24Buffer::diffL2(RGB24Buffer *buffer1, RGB24Buffer *buffer2)
{
    double sum = 0;
    int h = CORE_MIN(buffer1->h, buffer2->w);
    int w = CORE_MIN(buffer1->w, buffer2->w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            sum += (buffer1->element(i,j).toDouble() - buffer2->element(i,j).toDouble()).sumAllElementsSq() / 3.0;
        }
    }
    sum /= (double) h * w;
    return sum;
}

void RGB24Buffer::diffBuffer(RGB24Buffer *that, int *diffPtr)
{
    int sum = 0;
    int h = CORE_MIN(this->h, that->w);
    int w = CORE_MIN(this->w, that->w);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            RGBColor diff = RGBColor::diff(this->element(i,j), that->element(i,j));
            sum += diff.brightness();
            this->element(i,j) = diff;
        }
    }
    if (diffPtr != NULL) {
        *diffPtr = sum;
    }
}

RGB24Buffer *RGB24Buffer::diff(RGB24Buffer *buffer1, RGB24Buffer *buffer2, int *diff)
{
    RGB24Buffer *toReturn = new RGB24Buffer(buffer1);
    toReturn->diffBuffer(buffer2, diff);
    return toReturn;
}

} //namespace corecvs
