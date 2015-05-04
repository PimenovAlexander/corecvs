#include "geometryResultLayer.h"
#include <QPainter>


void GeometryResultLayer::drawImage(QImage *image)
{
    QPainter painter(image);
    for (int i = 0; i < rects.size(); i++)
    {
        painter.drawRect(rects[i]);
    }
}

void GeometryResultLayer::print() const
{
    printf("    Rects: %d \n", rects.size());

}
