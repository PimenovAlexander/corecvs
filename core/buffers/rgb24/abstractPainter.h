#pragma once
/**
 * \file abstractPainter.h
 *
 * \date Nov 22, 2012
 **/

#include <stdint.h>
#include <math.h>
#include <vector>

#include "core/stats/graphData.h"

#include "core/utils/global.h"
#include "core/buffers/rgb24/hardcodeFont.h"
#include "core/buffers/rgb24/hersheyVectorFont.h"
#include "core/buffers/rgb24/rgbColor.h"
#include "core/geometry/polygons.h"
#include "core/geometry/conic.h"
#include "core/geometry/line.h"
#include "core/geometry/ellipse.h"

namespace corecvs {


template<class TargetBuffer>
class AbstractPainter
{
    TargetBuffer *mTarget;
public:
    typedef typename TargetBuffer::InternalElementType ElementType;

    AbstractPainter(TargetBuffer *target) :
        mTarget(target)
    {}

    /**
     *  This function draws the char with the hardcoded font
     *  Not all chars are supported
     *
     *  Note: this method must be implemented right here, otherwise msvc couldn't understand correctly usage of "ElementType".
     **/
    void drawChar  (uint16_t x, uint16_t y, uint8_t theChar, ElementType color, int scale = 2)
    {
        uint8_t *char_ptr = NULL;
        if (theChar == ' ')
            return;
        if (theChar >='0' && theChar <= '9')
            char_ptr = HardcodeFont::num_glyphs + HardcodeFont::GLYPH_HEIGHT * (theChar - '0');
        if (theChar >='a' && theChar <= 'z')
            char_ptr = HardcodeFont::alpha_glyphs + HardcodeFont::GLYPH_HEIGHT * (theChar - 'a');
        if (theChar >='A' && theChar <= 'Z')
            char_ptr = HardcodeFont::alpha_glyphs + HardcodeFont::GLYPH_HEIGHT * (theChar - 'A');

        if (theChar == '-')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 0;
        if (theChar == '%')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 1;
        if (theChar == '.')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 2;
        if (theChar == ':')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 3;
        if (theChar == '!')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 4;
        if (theChar == '?')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 5;
        if (theChar == '$')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 6;
        if (theChar == '\"')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 7;
        if (theChar == '\'')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 8;
        if (theChar == '_')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 9;
        if (theChar == '(')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 11;
        if (theChar == ')')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 12;
        if (theChar == ',')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 13;
        if (theChar == '.')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 14;

        if (theChar == '[')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 15;
        if (theChar == ']')
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 16;


        if (char_ptr == NULL)
            char_ptr = HardcodeFont::symbols_glyphs + HardcodeFont::GLYPH_HEIGHT * 10;

        for (int i = 0; i < HardcodeFont::GLYPH_HEIGHT; i++)
        {
            for (int j = 0; j < HardcodeFont::GLYPH_WIDTH; j++)
            {
                if (char_ptr[i] & (1 << (HardcodeFont::GLYPH_WIDTH - j - 1)))
                {
                    int rx = x + scale * j;
                    int ry = y + scale * i;

                    for (int dy = 0; dy < scale; dy++)
                    {
                        for (int dx = 0; dx < scale; dx++)
                        {
                            mTarget->setElement(ry + dy, rx + dx, color);
                        }
                    }

                }
            }
        }
    }



    void drawFormat(uint16_t x, uint16_t y, ElementType color, int scale, const char *format, ...)
    {
        char str[1024];
        va_list marker;
        va_start(marker, format);
        vsnprintf(str, CORE_COUNT_OF(str), format, marker);

        int i = 0;
        while (str[i] != 0)
        {
            drawChar(x + i * 6 * scale, y, str[i], color, scale);
            i++;
        }
        va_end(marker);
    }

    int  drawGlyph(int16_t dx, int16_t dy, uint8_t theChar, ElementType color, int scale)
    {
        int id = 0;
        if (theChar >= ' ' || theChar <= '~')
        {
            id = theChar - ' ';
        }

        HersheyVectorFont::Glyph *glyph = &HersheyVectorFont::data[id];
        int oldx = -1;
        int oldy = -1;
        //printf("(%d)\n", glyph->len);

        for (int i = 0; i < glyph->len; i++)
        {
            int x = glyph->data[i * 2];
            int y = glyph->data[i * 2 + 1];

          //printf("[%d %d][%d %d]\n", x,y, oldx, oldy);
            if (x != -1 && y != -1 && oldx != -1 && oldy != -1)
            {
                mTarget->drawLine(dx + oldx * scale, dy - oldy * scale, dx + x * scale, dy - y * scale, color);
            }
            oldx = x;
            oldy = y;
        }
        return glyph->width * scale;
    }


    void drawFormatVector(int16_t x, int16_t y, ElementType color, int scale, const char *format, ...)
    {
        char str[1024];
        va_list marker;
        va_start(marker, format);
        vsnprintf(str, CORE_COUNT_OF(str), format, marker);

        int i = 0;
        int dx = 0;
        while (str[i] != 0)
        {
            dx += drawGlyph(x + dx, y, str[i++], color, scale);
        }
        va_end(marker);
    }

    void drawCircle(int x, int y, int rad, ElementType color )
    {
        int radsq = rad * rad;
        for (int dy = -rad; dy <= 0; dy++)
        {
            int dx = (int)sqrt((float)(radsq - (dy - 0.5) * (dy - 0.5)));
            for (int j= x - dx; j <= x + dx; j++)
            {
                mTarget->setElement(y - dy, j, color);
                mTarget->setElement(y + dy, j, color);
            }
        }
    }

    void drawCircle(const Circle2d &circle, RGBColor color)
    {
        drawCircle(circle.c.x(), circle.c.y(), circle.r, color);
    }

    void drawCircle(const Vector2dd &center, double radius, RGBColor color)
    {
        drawCircle(center.x(), center.y(), radius, color);
    }

    void drawEllipse(double x, double y, double rx, double ry, double ang, RGBColor color)
    {
        Ellipse ellipse;

        ellipse.center = Vector2dd(x, y);
        ellipse.axis   = Vector2dd(rx, ry);
        ellipse.angle  = degToRad(ang);

        drawEllipse(ellipse, color);
    }

    void drawEllipse(const Ellipse &ellipse, RGBColor color)
    {
        EllipseSpanIterator outer(ellipse);
        while (outer.hasValue())
        {
            HLineSpanInt span = outer.getSpan();
            for (int s1 = span.x1; s1 < span.x2; s1++ )
            {
                if (mTarget->isValidCoord(span.y(), s1))
                {
                    mTarget->element(span.y(), s1) = color;
                }
            }
            outer.step();
        }
    }

    class EqualPredicate
    {
    private:
        ElementType mValue;
        ElementType mReplace;

    public:
        /*int countPred;
        int countMark;
        int doubleMark;*/

        EqualPredicate(const ElementType &value, const ElementType &replace) :
            mValue(value)
        ,   mReplace(replace)
        /*,   countPred(0)
        ,   countMark(0)
        ,   doubleMark(0)*/
        {}

        bool operator()(TargetBuffer *buffer, int x, int y) {
            //countPred++;
            return buffer->element(y,x) == mValue;
        }

        void mark(TargetBuffer *buffer, int x, int y) {
            /*countMark++;
            if (buffer->element(y,x) == mReplace) {
                doubleMark++;
                buffer->element(y,x) = RGBColor::Yellow();
            } else {*/
                buffer->element(y,x) = mReplace;
            /*}*/
        }

    };

    struct Segment {
        int x1;
        int x2;
        int y;

        Segment() {}
        Segment(int _x1, int _x2, int _y) :
            x1(_x1), x2(_x2), y(_y) {}
    };

    template<class Predicate>
    void floodFill(int startX, int startY, Predicate &predicate)
    {

        std::vector<Segment> queue;

        /* Put the first segment*/
        if (!predicate(mTarget, startX, startY)) {
            return;
        }

//        printf("Pushing top %d [%d -> %d]\n", startY, startX, startX);
        queue.push_back(Segment(startX, startX, startY));

        while (!queue.empty()) {
            Segment state = queue.back();
            queue.pop_back();

            int x1 = state.x1;
            int x2 = state.x2;
            int y  = state.y;
            if (y < 0 || y > mTarget->h - 1) {
                continue;
            }

            while((x1 > 0) && predicate(mTarget, x1 - 1, y))
            {
                x1--;
            }

            while((x2 < mTarget->w - 1) && predicate(mTarget, x2 + 1, y) )
            {
                x2++;
            }

            //printf("Filling %d to %d\n", x1, x2);

            bool topAdd = false;    /* we store the fact that top scanline is already added */
            Segment top;
            top.y = y - 1;
            bool bottomAdd = false;    /* we store the fact that top scanline is already added */
            Segment bottom;
            bottom.y = y + 1;

            /* Fill line */
            for (int run = x1; run <= x2; run ++)
            {
                bool topFill    = false;
                bool bottomFill = false;

                if (y > 0) {
                    topFill = predicate(mTarget, run, y - 1);
                    if (!topAdd && topFill) {
                        topAdd = true;
                        top.x1 = run;
                    }
                    if (topAdd && (!topFill || run == x2)) {
                        topAdd = false;
                        top.x2 = run - 1;
//                        printf("Pushing top %d [%d -> %d]\n", top.y, top.x1, top.x2);
                        queue.push_back(top);
                    }
                }

                if (y < mTarget->h - 1 )
                {
                    bottomFill = predicate(mTarget, run, y + 1);
                    if (!bottomAdd && bottomFill) {
                        bottomAdd = true;
                        bottom.x1 = run;
                    }
                    if (bottomAdd && (!bottomFill || run == x2)) {
                        bottomAdd = false;
                        bottom.x2 = run - 1;
//                        printf("Pushing bottom %d [%d -> %d]\n", bottom.y, bottom.x1, bottom.x2);
                        queue.push_back(bottom);
                    }
                }

                predicate.mark(mTarget, run, y);

            }
        }

    }

    void drawLineD(double x1, double y1, double x2, double y2, ElementType color)
    {
        mTarget->drawLine(fround(x1), fround(y1), fround(x2), fround(y2), color);
    }

    void drawPath(const PointPath &pp, ElementType color)
    {
        if (pp.empty())
            return;

        if (pp.size() == 1)
        {
            drawLineD(pp[0].x(), pp[0].y(), pp[0].x(), pp[0].y(), color);
        }
        for (unsigned i = 0; i < pp.size() - 1; i++)
        {
            drawLineD(pp[i].x(), pp[i].y(), pp[i + 1].x(), pp[i + 1].y(), color);
        }
    }

    void drawPolygon(const Polygon &p, ElementType color)
    {
        if (p.empty())
            return;

        if (p.size() == 1)
        {
            drawLineD(p[0].x(), p[0].y(), p[0].x(), p[0].y(), color);
        }
        for (unsigned i = 0; i < p.size() - 1; i++)
        {
            drawLineD(p[i].x(), p[i].y(), p[i + 1].x(), p[i + 1].y(), color);
        }
        drawLineD(p[p.size() - 1].x(), p[p.size() - 1].y(), p[0].x(), p[0].y(), color);
    }

    void drawHLine(int x1, int y1, int x2, const ElementType &color)
    {
        drawSpan(HLineSpanInt(y1, x1, x2), color);
    }

    void drawSpan(const HLineSpanInt &span, const ElementType &color)
    {
        HLineSpanInt tspan = span;
        tspan.clip(mTarget->w, mTarget->h);
        for (int j = tspan.x1; j < tspan.x2; j++)
        {
            mTarget->element(tspan.y(), j) = color;
        }
    }

    void drawLine2d(const Line2d& line, const ElementType &color, double drawNormal = 0.0)
    {
        Rectangled area(0.0, 0.0, mTarget->getW() - 1, mTarget->getH() - 1);
        Ray2d ray = line.toRay();
        double t1 = 0.0;
        double t2 = 0.0;
        bool result = ray.clip(area.toConvexPolygon(), t1, t2);
        if (!result) {
            return;
        }
        mTarget->drawLine(ray.getPoint(t1), ray.getPoint(t2), color);
        if (drawNormal != 0.0) {
            Vector2dd middle = ray.getPoint((t1+t2) / 2.0);
            mTarget->drawLine(middle, middle + line.normal().normalised() * drawNormal, color);
        }

    }

    void drawGraph(const GraphData &data) {
    }

    virtual ~AbstractPainter() {}
};

/* TODO: try msvc to feed this...
template<class TargetBuffer>
void AbstractPainter<TargetBuffer>::drawFormatVector(int16_t x, int16_t y, AbstractPainter<TargetBuffer>::ElementType color, int scale, const char *format, ...)
{
    char str[1024];
    va_list marker;
    va_start(marker, format);
    vsnprintf(str, CORE_COUNT_OF(str), format, marker);

    int i = 0;
    int dx = 0;
    while (str[i] != 0)
    {
        dx += drawGlyph(x + dx, y, str[i++], color, scale);
    }
    va_end(marker);
}
*/



} /* namespace corecvs */

/* EOF */
