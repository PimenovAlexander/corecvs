#pragma once
/**
 * \file abstractPainter.h
 *
 * \date Nov 22, 2012
 **/

#include <stdint.h>
#include <math.h>
#include <vector>

#include "global.h"
#include "hardcodeFont.h"
#include "hersheyVectorFont.h"
#include "rgbColor.h"
#include "polygons.h"
#include "conic.h"

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
                            if (mTarget->isValidCoord(ry + dy, rx + dx))
                                mTarget->element(ry + dy, rx + dx) = color;
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
                if (mTarget->isValidCoord(y - dy, j))
                    mTarget->element(y - dy, j) = color;
                if (mTarget->isValidCoord(y + dy, j))
                    mTarget->element(y + dy, j) = color;
            }
        }
    }

    void drawCircle(const Circle2d &circle, RGBColor color)
    {
        drawCircle(circle.c.x(), circle.c.y(), circle.r, color);
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


    void drawPolygon(const Polygon &p, ElementType color )
    {
        for (unsigned i = 0; i < p.size() - 1; i++ )
        {
            mTarget->drawLine(p[i].x(), p[i].y(), p[i + 1].x(), p[i + 1].y(), color);
        }
        mTarget->drawLine(p[p.size() - 1].x(), p[p.size() - 1].y(), p[0].x(), p[0].y(), color);
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
