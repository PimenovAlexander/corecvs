#pragma once

#include "abstractFlowColorer.h"

class StereoColorer : public AbstractFlowColorer
{
public:
    StereoColorer(int preDisp, int minDisp, int maxDisp, int clickDisp);
  /*  StereoColorer(const ZoneParameters &zone);*/


    virtual QColor   getColor (Vector2d16 &point);
    virtual uint32_t getPacked(Vector2d16 &point);


    bool shouldDrawEllipses(int disp);
    QColor ellipsesColor();

private:
    int mPreDisp;
    int mMinDisp;
    int mMaxDisp;
    int mClickDisp;
};
