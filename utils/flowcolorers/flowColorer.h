#pragma once

#include "abstractFlowColorer.h"

class FlowColorer : public AbstractFlowColorer
{
public:
    FlowColorer(const DrawColoring _style = drawHeatColoring) :
        AbstractFlowColorer(_style ) {};

    virtual QColor   getColor (Vector2d16 &point);
    virtual uint32_t getPacked(Vector2d16 &point);

    bool shouldDrawEllipses(int disp);
    QColor ellipsesColor();
};
