/**
 * \file painterHelpers.cpp
 * \brief Add Comment Here
 *
 * \date Nov 5, 2011
 * \author alexander
 */

#include <stdint.h>

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
