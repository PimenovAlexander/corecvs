/**
 * \file testbedImageWidget.cpp
 *
 * \date Dec 2, 2013
 **/

#include <QtGui/qpainter.h>
#include "testbedImageWidget.h"

TestbedImageWidget::TestbedImageWidget(QWidget *parent, bool showHeader) :
    AdvancedImageWidget(parent, showHeader)
{
    decorationSize = 100;
    mUi->widget->setMouseTracking(true);
}

void TestbedImageWidget::childMouseMoved   (QMouseEvent  *event)
{
    mousePoint = event->pos();
    AdvancedImageWidget::childMouseMoved(event);
    mUi->widget->update();
}

void TestbedImageWidget::childRepaint(QPaintEvent *event, QWidget *who)
{
    AdvancedImageWidget::childRepaint(event, who);

    if (mCurrentToolClass == POINT_SELECTION_TOOLS &&
        (mCurrentPointButton == 4 ||
        mCurrentPointButton == 5 ))
    {
        QPainter painter(who);
        QPen pen;
        QVector<qreal> dashesEven;
        QVector<qreal> dashesOdd;
        dashesEven << 2 << 1 << 2 << 1;
        dashesOdd  << 1 << 2 << 1 << 2;
        pen.setDashPattern(dashesEven);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawEllipse(mousePoint, decorationSize / 2, decorationSize / 2);
        pen.setDashPattern(dashesOdd);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.drawEllipse(mousePoint, decorationSize / 2, decorationSize / 2);
    }
}

int TestbedImageWidget::getImageRadius()
{
    QPoint center = widgetToImage(QPoint(0,0));
    QPoint dist   = widgetToImage(QPoint(decorationSize,0));

    return abs(dist.x() - center.x()) / 2;
}
