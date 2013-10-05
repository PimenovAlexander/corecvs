#include "flowDrawer.h"
#include <QDebug>

void FlowDrawer::draw(QPainter &painter, FlowBuffer *flow, DrawMode mode, AbstractFlowColorer &colorer)
{
    if (mode == drawNothing)
        return;

    for (int i = 0; i < flow->h; i++)
    {
        for (int j = 0; j < flow->w; j++)
        {
            if (!flow->isElementKnown(i,j))
                continue;
            Vector2d16 &point = flow->element(i,j);

            int x2 = j + point.x();
            int y2 = i + point.y();
            painter.setPen(colorer.getColor(point));


            if ( mode == drawLines ||
                 mode == drawLinesAndDots ||
                (mode == drawLongLinesOnly  && (point.l1Metric() > 10))
               )
            {
                    painter.drawLine(j, i, x2, y2);
            }

            if (mode == drawDots || mode == drawLinesAndDots)
            {
                int x1 = j;
                int y1 = i;
                int disp = static_cast<int>(point.x());

                painter.drawRect(x1-1, y1-1, 2, 2);

                if (colorer.shouldDrawEllipses(disp))
                {
                    painter.setPen(colorer.ellipsesColor());
                    painter.drawEllipse(x1-2, y1-2, 4, 4);
                }
            }

        }
    }
}


void FlowDrawer::drawDotsFast(QImage &image, const QPoint &startPoint, FlowBuffer *flow, AbstractFlowColorer &colorer)
{
    if (image.format() != QImage::Format_RGB32)
    {
       return;
    }

    int imageX = 0;
    int imageY = 0;
    int flowX  = 0;
    int flowY  = 0;

    if (startPoint.x() > 0) {
        imageX = startPoint.x();
    } else {
        flowX = -startPoint.x();
    }

    if (startPoint.y() > 0) {
        imageY = startPoint.y();
    } else {
        flowY = -startPoint.y();
    }

    int h = std::min (image.height() - imageY, flow->h - flowY);
    int w = std::min (image.width()  - imageX, flow->w - flowX);

    for (int i = 0; i < h; i++)
    {
        uint32_t    *imageRunner = ((uint32_t *)image.scanLine(i + imageY)) + imageX;
        FlowElement *flowRunner  = &flow->element(i + flowY,flowX);

        for (int j = 0; j < w; j++,  flowRunner++, imageRunner++)
        {
            if (flowRunner->x() == (int)FlowBuffer::FLOW_UNKNOWN_X ||
                flowRunner->y() == (int)FlowBuffer::FLOW_UNKNOWN_Y )
                continue;

            Vector2d16 shift(flowRunner->x(), flowRunner->y());

            *imageRunner = colorer.getPacked(shift);
        }
    }
}

void FlowDrawer::drawBigDotsFast(QImage &image, const QPoint &startPoint, FlowBuffer *flow, AbstractFlowColorer &colorer)
{
    if (image.format() != QImage::Format_RGB32)
    {
       return;
    }

    const int borderSize = 1;

    int imageX = borderSize;
    int imageY = borderSize;
    int flowX  = borderSize;
    int flowY  = borderSize;

    if (startPoint.x() > borderSize) {
        imageX = startPoint.x();
    } else {
        flowX = borderSize - startPoint.x();
    }

    if (startPoint.y() > borderSize) {
        imageY = startPoint.y();
    } else {
        flowY = borderSize - startPoint.y();
    }

    int h = std::min (image.height() - imageY - 1, flow->h - flowY);
    int w = std::min (image.width()  - imageX - 1, flow->w - flowX);

    for (int i = 0; i < h; i++)
    {
       // This should be optimized
        uint32_t    *imageRunner = ((uint32_t *)image.scanLine(i + imageY)) + imageX;
       // According to doc this should be divisible by 4
        int stride = image.bytesPerLine() / sizeof(uint32_t);

        FlowElement *flowRunner  = &flow->element(i + flowY, flowX);

        for (int j = 0; j < w; j++,  flowRunner++, imageRunner++)
        {
            if (flowRunner->x() == (int)FlowBuffer::FLOW_UNKNOWN_X ||
                flowRunner->y() == (int)FlowBuffer::FLOW_UNKNOWN_Y )
                continue;

            Vector2d16 shift(flowRunner->x(), flowRunner->y());
            uint32_t color = colorer.getPacked(shift);
            *(imageRunner - 1 - stride) = color;
            *(imageRunner + 0 - stride) = color;
            *(imageRunner + 1 - stride) = color;

            *(imageRunner - 1         ) = color;
            *(imageRunner + 1         ) = color;

            *(imageRunner - 1 + stride) = color;
            *(imageRunner + 0 + stride) = color;
            *(imageRunner + 1 + stride) = color;
        }
    }
}

