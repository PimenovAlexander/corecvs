/**
 * \file painterHelpers.cpp
 * \brief Add Comment Here
 *
 * \date Nov 5, 2011
 * \author alexander
 */

#include <stdint.h>
#include <QPainterPath>


#include "painterHelpers.h"
#include "mathUtils.h"



void drawLine(QPainter& painter, const Vector2dd &start, const Vector2dd &end)
{
    painter.drawLine(
            fround(start.x()), fround(start.y()),
            fround(end  .x()), fround(end  .y()));
}

void drawCircle(QPainter& painter, const Vector2dd &center, double radius)
{
    painter.drawEllipse(
            fround(center.x() - radius),
            fround(center.y() - radius),
            fround(2.0 * radius), fround(2.0 * radius));
}

void drawSquare(QPainter& painter, const Vector2dd &center, int size)
{
    painter.drawRect(
            center.x() - (size / 2),
            center.y() - (size / 2),
            size,
            size
    );
}


void drawArc(QPainter& painter, const Vector2dd &center, double radius, int start, int len)
{
    painter.drawArc(
            fround(center.x() - radius),
            fround(center.y() - radius),
            fround(2.0 * radius), fround(2.0 * radius),
            start, len);
}

void drawPie(QPainter& painter, const Vector2dd &center, double radius, int start, int len)
{
    painter.drawPie(
            fround(center.x() - radius),
            fround(center.y() - radius),
            fround(2.0 * radius), fround(2.0 * radius),
            start, len);
}

void uniteAnaglyph (const QImage *left, const QImage *right, QImage *result, uint32_t lmask, uint32_t rmask)
{
    if (left  ->format() != right ->format() ||
        left  ->format() != result->format() )
        return;

    int hmin = left  ->height();
    if (hmin > right ->height()) hmin = right ->height();
    if (hmin > result->height()) hmin = result->height();

    int wmin = left  ->width();
    if (wmin > right ->width()) wmin = right ->width();
    if (wmin > result->width()) wmin = result->width();

    for (int i = 0; i < hmin; i++)
    {
        const uint32_t *src1 = (const uint32_t *)left ->scanLine(i);
        const uint32_t *src2 = (const uint32_t *)right->scanLine(i);
        uint32_t *dest       = (uint32_t *)result->scanLine(i);

       for (int j = 0; j < wmin; j++)
       {
           *dest = (*src1 & lmask) | (*src2 & rmask);
           dest++;
           src1++;
           src2++;
       }
    }

}

void disableComboBoxItem(QComboBox *box, int item) {
    QModelIndex index1 = box->model()->index(item,0);
    QVariant v1(0);
    box->model()->setData( index1, v1, Qt::UserRole -1);
}


QRect fixedAspectRescaleRect (QRect source, QRect target)
{
    source = source.normalized();
    target = target.normalized();
    if (source.isEmpty() || target.isEmpty())
        return QRect();

    double aspectClient = (double)target.width() / target.height();
    double aspectMask   = (double)source.width() / source.height();

    if (aspectMask > aspectClient) {
        /* Mask is narrower in horizontal direction. Margins on top and bottom*/
        double maskScaledHeight = (double)source.height() / source.width() * target.width();
        double margin = (target.height() - maskScaledHeight) / 2.0;
        return QRect(target.x() + 0, target.y() + margin, target.width(), maskScaledHeight);
    } else {
        /* Mask is narrower in vertical direction. Margins on right and left */
        double maskScaledWidth = (double)source.width() / source.height() * target.height();
        double margin = (target.width() - maskScaledWidth) / 2.0;
        return QRect(target.x() + margin, target.y() + 0, maskScaledWidth, target.height());
    }
}


void draw2Circle (QPainter& painter,
                   const Vector2dd &center1, int size1,
                   const Vector2dd &center2, int size2
                   )
{
    QPainterPath ppath;

    Vector2dd dir = (center2 - center1);
    double dist = dir.l2Metric();
    dir = dir / dist;
    double angle = dir.argument();

    QRectF circle1(center1.x() - size1, center1.y() - size1, 2.0 * size1, 2.0 * size1);
    QRectF circle2(center2.x() - size2, center2.y() - size2, 2.0 * size2, 2.0 * size2);

    /* Calculating touch line for the first circle */
    double dsize = size1 - size2;
    double dang =  acos (dsize / dist);

    /*-------*/
    double startang1 = radToDeg(angle - dang);
    if (startang1 > 360.0) startang1 -= 360.0;
    if (startang1 < 0    ) startang1 += 360.0;

    double arcspan1  = 360.0 - radToDeg(2 * dang);

    double startang2 = radToDeg((angle + M_PI) - ( M_PI - dang));
    if (startang2 > 360.0) startang2 -= 360.0;
    if (startang2 < 0    ) startang2 += 360.0;
    double arcspan2  = 360.0 - radToDeg(2 * ( M_PI - dang));

    Vector2dd start, end;

    /*------**/
    ppath.arcMoveTo(circle1, -startang1);
    ppath.arcTo    (circle1, -startang1, arcspan1);

    start = center1 + Vector2dd::FromPolar(degToRad(startang1 - arcspan1), size1);
    end   = center2 + Vector2dd::FromPolar(degToRad(startang2),            size2);
//    ppath.moveTo(start.x(), start.y());
    ppath.lineTo(end.x(), end.y());

//    ppath.arcMoveTo(circle2, -startang2);
    ppath.arcTo    (circle2, -startang2, arcspan2);

    start = center2 + Vector2dd::FromPolar(degToRad(startang2 - arcspan2), size2);
    end   = center1 + Vector2dd::FromPolar(degToRad(startang1), size1);
//    ppath.moveTo(start.x(), start.y());
    ppath.lineTo(end.x(), end.y());
    ppath.closeSubpath();
    painter.drawPath(ppath);
}


void drawMetaballs(QPainter &painter, const Vector2dd &center1, int size1, const Vector2dd &center2, int size2)
{
    QPainterPath ppath;

#if 0
    ppath.addEllipse(center1.x() - size1, center1.y() - size1, 2.0 * size1, 2.0 * size1);
    ppath.addEllipse(center2.x() - size2, center2.y() - size2, 2.0 * size2, 2.0 * size2);

    Vector2dd dir = (center2 - center1).normalised();
    Vector2dd side = dir.leftNormal();

    Vector2dd center = (center1 + center2) / 2.0;

    Vector2dd start = center1 + side * size1;
    Vector2dd end = center2 + side * size2;

    ppath.moveTo(start.x(), start.y());
    ppath.cubicTo(center.x(), center.y(), center.x(), center.y(), end.x(), end.y());

    start = center1 - side * size1;
    end = center2 - side * size2;

    ppath.moveTo(start.x(), start.y());
    ppath.cubicTo(center.x(), center.y(), center.x(), center.y(), end.x(), end.y());
#else
    Vector2dd dir = (center2 - center1);
    double dist = dir.l2Metric();
    dir = dir / dist;
    double angle = dir.argument();
    Vector2dd side = dir.leftNormal().normalised();

    Vector2dd centerr = center1 + dir * ((dist - size1 - size2) / 2.0 + size1) ;
    Vector2dd centerl = center1 + dir * ((dist - size1 - size2) / 2.0 + size1) ;


    QRectF circle1(center1.x() - size1, center1.y() - size1, 2.0 * size1, 2.0 * size1);
    QRectF circle2(center2.x() - size2, center2.y() - size2, 2.0 * size2, 2.0 * size2);


    /* Calculating touch line for the first circle */
    Vector2dd diam1 = centerr - center1;
    double l1 = diam1.l2Metric();
    double dang1 = acos (size1 / l1);

    Vector2dd diam2 = centerr - center2;
    double l2 = diam2.l2Metric();
    double dang2 = acos (size2 / l2);

    /*-------*/
    double startang1 = radToDeg(angle - dang1);
    if (startang1 > 360.0) startang1 -= 360.0;
    if (startang1 < 0    ) startang1 += 360.0;

    double arcspan1  = 360.0 - radToDeg(2 * dang1);

    double startang2 = radToDeg(angle + M_PI - dang2);
    if (startang2 > 360.0) startang2 -= 360.0;
    if (startang2 < 0    ) startang2 += 360.0;
    double arcspan2  = 360.0 - radToDeg(2 * dang2);

    Vector2dd start, end;

    /*------**/
    ppath.arcMoveTo(circle1, -startang1);
    ppath.arcTo    (circle1, -startang1, arcspan1);

    start = center1 + Vector2dd::FromPolar(degToRad(startang1 - arcspan1), size1);
    end   = center2 + Vector2dd::FromPolar(degToRad(startang2),            size2);
//    ppath.moveTo(start.x(), start.y());
    ppath.cubicTo(centerr.x(), centerr.y(), centerr.x(), centerr.y(), end.x(), end.y());

//    ppath.arcMoveTo(circle2, -startang2);
    ppath.arcTo    (circle2, -startang2, arcspan2);

    start = center2 + Vector2dd::FromPolar(degToRad(startang2 - arcspan2), size2);
    end   = center1 + Vector2dd::FromPolar(degToRad(startang1), size1);
//    ppath.moveTo(start.x(), start.y());
    ppath.cubicTo(centerr.x(), centerr.y(), centerr.x(), centerr.y(), end.x(), end.y());
    ppath.closeSubpath();



#endif

    painter.drawPath(ppath);
}

void drawMetaballs1(QPainter &painter, const Vector2dd &center1, int r1, const Vector2dd &center2, int r2)
{

    QRectF circle1(center1.x() - r1, center1.y() - r1, 2.0 * r1, 2.0 * r1);
    QRectF circle2(center2.x() - r2, center2.y() - r2, 2.0 * r2, 2.0 * r2);

    Vector2dd dir = (center2 - center1);
    double dist = dir.l2Metric();
    dir = dir / dist;
    double angle = dir.argument();
    Vector2dd side = dir.leftNormal().normalised();

    /* Lets check how the circles are positioned */


    if (dist < fabs(r1 - r2))
    {
        if (r1 >  r2)
        {
            painter.drawEllipse(circle1);
        } else {
            painter.drawEllipse(circle2);
        }
    } else {
        QPainterPath ppath;

        //double toMiddle = (dist - r1 - r2) / 2.0 + r1;
        double toMiddle = ((dist * dist) + r1*r1 - r2*r2) / (2 * dist);
        double elevated = 0.0;

        if (toMiddle < r1) elevated = sqrt(r1 * r1 - toMiddle * toMiddle);
        elevated += 10;

        Vector2dd centerr = center1 + dir * toMiddle + side * elevated;
        Vector2dd centerl = center1 + dir * toMiddle - side * elevated;

     /*   painter.setPen(Qt::cyan);
        painter.drawEllipse(circle1);
        painter.drawEllipse(circle2);
        painter.setPen(Qt::black);*/

        /* Calculating the angle of touch line to the first circle */
        Vector2dd diam1 = centerr - center1;
        double l1 = diam1.l2Metric();
        double dang1 = acos (r1 / l1) + atan2(elevated, toMiddle);

        /* Calculating touch line for the second circle */

        Vector2dd diam2 = centerl - center2;
        double l2 = diam2.l2Metric();
        double dang2 = acos (r2 / l2) + atan2(elevated, dist - toMiddle);

        /*-------*/
        double startang1 = radToDeg(angle - dang1);
        if (startang1 > 360.0) startang1 -= 360.0;
        if (startang1 < 0    ) startang1 += 360.0;

        double arcspan1  = 360.0 - radToDeg(2 * dang1);

        double startang2 = radToDeg(angle + M_PI - dang2);
        if (startang2 > 360.0) startang2 -= 360.0;
        if (startang2 < 0    ) startang2 += 360.0;
        double arcspan2  = 360.0 - radToDeg(2 * dang2);

        Vector2dd start, end;

        /*------**/
        ppath.arcMoveTo(circle1, -startang1);
        ppath.arcTo    (circle1, -startang1, arcspan1);

        start = center1 + Vector2dd::FromPolar(degToRad(startang1 - arcspan1), r1);
        end   = center2 + Vector2dd::FromPolar(degToRad(startang2),            r2);
    //    ppath.moveTo(start.x(), start.y());
        ppath.cubicTo(centerr.x(), centerr.y(), centerr.x(), centerr.y(), end.x(), end.y());

    //    ppath.arcMoveTo(circle2, -startang2);
        ppath.arcTo    (circle2, -startang2, arcspan2);

        start = center2 + Vector2dd::FromPolar(degToRad(startang2 - arcspan2), r2);
        end   = center1 + Vector2dd::FromPolar(degToRad(startang1), r1);
    //    ppath.moveTo(start.x(), start.y());
        ppath.cubicTo(centerl.x(), centerl.y(), centerl.x(), centerl.y(), end.x(), end.y());
        ppath.closeSubpath();

//        painter.drawEllipse(centerr.x() - 2, centerr.y() - 2, 5, 5);
//        painter.drawEllipse(centerl.x() - 2, centerl.y() - 2, 5, 5);

        painter.drawPath(ppath);
    }

}
