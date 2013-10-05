#include "flowColorer.h"
#include <QtCore/QDebug>



QColor FlowColorer::getColor (Vector2d16 &point)
{
    QColor color;
    if (style == drawBlue) {
        color = QColor(0, 0, 255);
    } else if (style == drawSimpleColoring)
    {
        int hue = (abs((int)point.x()) + abs((int)point.y())) * 300 / 50;
        if (hue > 255) hue = 255;
        color = hsvColorCache[hue];
    } else if (style == drawHeatColoring)
    {
        int hue = (abs((int)point.x()) + abs((int)point.y())) * 300 / 30;
        if (hue > 255) hue = 255;
        color = QColor(hue, 0, 255 - hue);
    }
    return color;
}

uint32_t FlowColorer::getPacked(Vector2d16 &point)
{
    if (style == drawSimpleColoring)
    {
        int hue = (abs((int)point.x()) + abs((int)point.y())) * 300 / 50;
        if (hue > 255) hue = 255;
        return hsvPackedCache[hue];
    } else if (style == drawHeatColoring)
    {
        int hue = (abs((int)point.x()) + abs((int)point.y())) * 300 / 30;
        if (hue > 255) hue = 255;
        return (hue << 16) | (255 - hue);
    }
    return packRGB(0,0,0xFF);
}

bool FlowColorer::shouldDrawEllipses(int disp)
{
    Q_UNUSED(disp)
    return false;
}

QColor FlowColorer::ellipsesColor()
{
    return QColor(0, 0, 255);
}
