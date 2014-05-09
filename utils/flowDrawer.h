#pragma once

#include <QtGui/QPainter>

#include "flowBuffer.h"
#include "flowcolorers/abstractFlowColorer.h"

enum DrawMode {
    drawNothing,
    drawDots,
    drawDotsFast,
    drawDotsFastBig,
    drawLines,
    drawLongLinesOnly,
    drawLinesAndDots,
};


class FlowDrawer
{
public:
    static void draw(QPainter &painter, FlowBuffer *flow, DrawMode mode, AbstractFlowColorer &colorer);

    static void drawDotsFast(QImage &image, const QPoint &startPoint, FlowBuffer *flow, AbstractFlowColorer &colorer);
    static void drawBigDotsFast(QImage &image, const QPoint &startPoint, FlowBuffer *flow, AbstractFlowColorer &colorer);
protected:


};

