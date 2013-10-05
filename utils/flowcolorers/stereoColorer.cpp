#include "stereoColorer.h"
//#include "generatedParameters/zoneParameters.h"

StereoColorer::StereoColorer(int preDisp, int minDisp, int maxDisp, int clickDisp)
    : AbstractFlowColorer(),
      mPreDisp(preDisp),
      mMinDisp(minDisp),
      mMaxDisp(maxDisp),
      mClickDisp(clickDisp)
{
}

/*
StereoColorer::StereoColorer(const ZoneParameters &zone)
: AbstractFlowColorer(),
  mPreDisp(zone.preDisp()),
  mMinDisp(zone.minDisp()),
  mMaxDisp(zone.maxDisp()),
  mClickDisp(zone.clickDisp())
{

}
*/

QColor StereoColorer::getColor (Vector2d16 &point)
{
    int disp = static_cast<int>(point.x());

    if ((point.y() > 20) || (point.y() < -20)) {
        return  QColor(0, 0, 0);
    }
    if (disp < mPreDisp) {
        return QColor(0, 128, 0);
    }
    if (disp < mMinDisp) {
        return QColor(248, 168, 255);
    }
    if (disp > mMaxDisp ) {
        return QColor(128, 0, 0);
    }

    int hue = (disp - mMinDisp) * 255;
    int diff = mMaxDisp - mMinDisp;
    hue = hue / ((diff > 0) ? diff : 1);
    if (hue > 255) hue = 255;
    if (hue < 0) hue = 0;
    return hsvColorCache[hue];
}


uint32_t StereoColorer::getPacked(Vector2d16 &point)
{
    int disp = static_cast<int>(point.x());

    if ((point.y() > 20) || (point.y() < -20)) {
        return packRGB(0x00,0x00,0x00);
    }
    if (disp < mPreDisp) {
        return packRGB(0x00,0x7F,0x00);
    }
    if (disp < mMinDisp) {
        return packRGB(248, 168, 255);
    }
    if (disp > mMaxDisp ) {
        return packRGB(0x7F,0x00,0x00);
    }

    int hue = (disp - mMinDisp) * 255;
    int diff = mMaxDisp - mMinDisp;
    hue = hue / ((diff > 0) ? diff : 1);
    if (hue > 255) hue = 255;
    if (hue < 0) hue = 0;
    return hsvPackedCache[hue];
}


bool StereoColorer::shouldDrawEllipses(int disp)
{
    return (disp > mMaxDisp && disp < mClickDisp);
}

QColor StereoColorer::ellipsesColor()
{
    return QColor(255, 0, 0);
}

