/**
 * \file rgb24Buffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 1, 2010
 * \author alexander
 */
#include "global.h"

#include "rectangle.h"
#include "rgb24Buffer.h"
#include "hardcodeFont.h"

#undef rad2     // it's defined at win hdrs

namespace corecvs {


void RGB24Buffer::drawG12Buffer(G12Buffer *src, int32_t y, int32_t x)
{
    ASSERT_TRUE((this->h == src->h && this->w == src->w), "Wrong sizes");
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

void RGB24Buffer::drawG8Buffer(G8Buffer *src, int32_t y, int32_t x)
{
    ASSERT_TRUE((this->h == src->h && this->w == src->w), "Wrong sizes");
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

/**
 * This function implements the Bresenham algorithm.
 * More details on the Bresenham algorithm here - http://en.wikipedia.org/wiki/Bresenham_algorithm
 *
 * NB! This function doesn't perform clipping it just draws nothing
 * You must clip yourself
 */
void RGB24Buffer::drawLineSimple (int x1, int y1, int x2, int y2, RGBColor color )
{
    if (x1 < 0 || y1 < 0 || x1 >= w || y1 >= h)
        return;
    if (x2 < 0 || y2 < 0 || x2 >= w || y2 >= h)
        return;

    int    dx = (x2 >= x1) ? x2 - x1 : x1 - x2;
    int    dy = (y2 >= y1) ? y2 - y1 : y1 - y2;
    int    sx = (x2 >= x1) ? 1 : -1;
    int    sy = (y2 >= y1) ? 1 : -1;
    int x;
    int y;
    int i;


    if ( dy <= dx )
    {
        int    d  = ( dy << 1 ) - dx;
        int    d1 = dy << 1;
        int    d2 = ( dy - dx ) << 1;

        element(y1, x1) = color;

        for (x = x1 + sx, y = y1, i = 1; i <= dx; i++, x += sx )
        {
            if ( d > 0 )
            {
                d += d2;
                y += sy;
            }
            else
            {
                d += d1;
            }

            element(y, x) = color;
        }
    }
    else
    {
        int    d  = ( dx << 1 ) - dy;
        int    d1 = dx << 1;
        int    d2 = ( dx - dy ) << 1;

        element(y1, x1) = color;

        for (x = x1, y = y1 + sy, i = 1; i <= dy; i++, y += sy )
        {
            if ( d > 0 )
            {
                d += d2;
                x += sx;
            }
            else
            {
                d += d1;
            }

            element(y, x) = color;
        }
    }
}


void RGB24Buffer::drawPixel ( int x, int y, RGBColor color)
{
    if (this->isValidCoord(y, x))
    {
        element(y, x) = color;
    }
}

void RGB24Buffer::drawLine(int x1, int y1, int x2, int y2, RGBColor color )
{
    Rectangle<int> rect;
    rect.corner = Vector2d<int>(0,0);
    rect.size   = Vector2d<int>(w - 1,h - 1);
    Vector2d32 lineStart(x1,y1);
    Vector2d32 lineEnd(x2,y2);
    rect.clipCohenSutherland(lineStart, lineEnd);
    drawLineSimple(lineStart.x(), lineStart.y(),lineEnd.x(), lineEnd.y(), color);
}

void RGB24Buffer::drawSprite(int x, int y, RGBColor color, int d[][2], int pointNum)
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
void RGB24Buffer::drawCrosshare1 (int x, int y, RGBColor color)
{
    int d[8][2] = {{1,1},{2,2},{-1,-1},{-2,-2},{-1,1},{-2,2},{1,-1},{2,-2}};
    this->drawSprite(x, y, color, d, 8);
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

void RGB24Buffer::drawCrosshare2 (int x, int y, RGBColor color)
{
    int d[16][2] = {{1,1},{2,2},{-1,-1},{-2,-2},{-1,1},{-2,2},{1,-1},{2,-2},
            {1,3},{3,1},{-1,-3},{-3,-1},{1,-3},{3,-1},{-1,3},{-3,1}};
    this->drawSprite(x, y, color, d, 16);
}

/**  Draws a marker at the point having the form of
 *  <pre>
 *   \#\#\#
 *   \#o\#
 *   \#\#\#
 *  </pre>
 */
void RGB24Buffer::drawCrosshare3 (int x, int y, RGBColor color)
{
    int d[8][2] = {{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1},{0,1}};
    this->drawSprite(x, y, color, d, 8);
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

#ifdef ASSERTS
            Vector2d32 res(vec);
            res += Vector2d32(j, i);
            if (!this->isValidCoord(res))
            {
                ASSERT_FAIL("Overflow in the flow");
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

void RGB24Buffer::drawCorrespondanceList(CorrespondanceList *src, double colorScaler, int32_t y, int32_t x)
{
    if (src == NULL) {
        return;
    }

    CorrespondanceList::iterator it;
    for (it = src->begin(); it != src->end(); ++it)
    {
        Correspondance &tmpCorr = (*it);
        Vector2dd from = tmpCorr.start;
        Vector2dd to = tmpCorr.end;
        Vector2dd vec = to - from;
        //RGBColor color = RGBColor(128 + (vec.x() * 5),128 + (vec.y() * 5), 0);
        RGBColor color = RGBColor::rainbow1(!vec / colorScaler);
        drawLineSimple(from.x() + x, from.y() + y, to.x() + x, to.y() + y, color);
        this->drawCrosshare3(to.x() + x, to.y() + y, color);
    }
}




#if 0
void RGB24Buffer::drawRectangle(const Rectangle<int32_t> &rect, RGBColor color, int style)
{
    if (style == 1)
    {
        this->drawCrosshare3(rect.corner.y()                ,rect.corner.x(), color);
        this->drawCrosshare3(rect.corner.y() + rect.size.y(),rect.corner.x(), color);
        this->drawCrosshare3(rect.corner.y()                ,rect.corner.x() + rect.size.x(), color);
        this->drawCrosshare3(rect.corner.y() + rect.size.y(),rect.corner.x() + rect.size.x(), color);
        return;
    }

    if (style == 0)
    {
        for (int i = 0; i < rect.size.y(); i++)
        {
            this->element(rect.corner.y() + i, rect.corner.x()) = color;
            this->element(rect.corner.y() + i, rect.corner.x() + rect.size.x()) = color;
        }

        for (int j = 1; j < rect.size.x() - 1; j++)
        {
            this->element(rect.corner.y(), rect.corner.x() + j) = color;
            this->element(rect.corner.y() + rect.size.y(), rect.corner.x() +  j) = color;
        }
    }



}
#endif

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


void RGB24Buffer::drawHistogram1024x512(Histogram *hist, int x, int y, uint16_t flags)
{
    int i,j,k;

    /**
     *  TODO: Add asserts
     **/

    int intervalStart = (flags & FLAGS_INCLUDE_MARGIN) ? 0 : 1;
    int intervalEnd   = (flags & FLAGS_INCLUDE_MARGIN) ? G12Buffer::BUFFER_MAX_VALUE : G12Buffer::BUFFER_MAX_VALUE - 1;



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
        int max  =  0;
        int min  =  0xFFFF;
        int sum =   0;
        int count = 4;

        for(j = 0; j < count; j++)
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


        for (k = 0; k < minColumn ; k++)
        {
            this->element(y + 512 - k,  i + x) = RGBColor(0xFF,0,0);
        }
        for (; k < sumColumn; k++)
        {
            this->element(y + 512 - k, i + x) = RGBColor(0, 0xFF,0);
        }
        for (; k < maxColumn; k++)
        {
            this->element(y + 512 - k, i + x) = RGBColor(0, 0, 0xFF);
        }
    }
}

// TODO: make this more pretty
void RGB24Buffer::drawArc(int x, int y, int rad, RGBColor color)
{
    int rad2 = rad * rad;
    int olddx = 0;
    for (int dy = -rad + 1; dy <= 0; dy++)
    {
        int newdx = (int)sqrt((float)(rad2 - (dy - 0.5) * (dy - 0.5)));

        for (int dx = olddx; dx <= newdx ; dx++) {
            if (this->isValidCoord(y - dy, x + dx))  this->element(y - dy, x + dx) = color;
            if (this->isValidCoord(y + dy, x + dx))  this->element(y + dy, x + dx) = color;
            if (this->isValidCoord(y - dy, x - dx))  this->element(y - dy, x - dx) = color;
            if (this->isValidCoord(y + dy, x - dx))  this->element(y + dy, x - dx) = color;
        }

        olddx = newdx;
    }
}



void RGB24Buffer::drawArc1(int x, int y, int rad, RGBColor color)
{
    int rad2 = rad * rad;
    int olddx = 0;
    for (int dy = -rad + 1; dy <= 0; dy++)
    {
        int newdx = (int)sqrt((float)(rad2 - (dy - 0.5) * (dy - 0.5)));

        for (int dx = olddx; dx <= newdx ; dx++) {
            if (this->isValidCoord(y - dy, x + dx))  this->element(y - dy, x + dx) = color;
            if (this->isValidCoord(y + dy, x + dx))  this->element(y + dy, x + dx) = color;
            if (this->isValidCoord(y - dy, x - dx))  this->element(y - dy, x - dx) = color;
            if (this->isValidCoord(y + dy, x - dx))  this->element(y + dy, x - dx) = color;
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


} //namespace corecvs

