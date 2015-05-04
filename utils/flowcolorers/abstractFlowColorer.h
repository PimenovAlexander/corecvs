#pragma once

#include <stdint.h>
#include <QColor>

#include "vector2d.h"

using namespace corecvs;

enum DrawColoring {
    drawHeatColoring,
    drawSimpleColoring,
    drawBlue
};

class AbstractFlowColorer
{
public:
    static QColor   *hsvColorCache;
    static uint32_t *hsvPackedCache;
    static int dummy;
    static int hsvCacheInit();

    DrawColoring style;

    AbstractFlowColorer(const DrawColoring _style = drawHeatColoring) :
        style(_style ) {};

    static uint32_t packRGB(unsigned r, unsigned g, unsigned b)
    {
        return (0xFF << 24) | (r << 16) | (g << 8) | (b);
    }

    virtual QColor   getColor (Vector2d16 &point) = 0;
    virtual uint32_t getPacked(Vector2d16 &point) = 0;

    virtual bool shouldDrawEllipses(int disp) = 0;
    virtual QColor ellipsesColor() = 0;
};

