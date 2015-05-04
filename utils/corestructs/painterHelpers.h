/**
 * \file painterHelpers.h
 * \brief Add Comment Here
 *
 * \date Nov 5, 2011
 * \author alexander
 */

#ifndef PAINTERHELPERS_H_
#define PAINTERHELPERS_H_

#include <stdint.h>
#include <QRect>
#include <QPainter>
#include <QComboBox>

#include "global.h"

#include "vector2d.h"
#include "rectangle.h"



using namespace corecvs;

class Core2Qt {
public:

    static QPoint QPointFromVector2d32(const Vector2d32 &point )
    {
        return QPoint(point.x(), point.y());
    }

    static QPointF QPointFromVector2dd(const Vector2dd &point )
    {
        return QPointF(point.x(), point.y());
    }

    static QRect QRectFromRectangle (const Rectangle32 &rect)
    {
        return QRect(
            rect.corner.x(),  rect.corner.y(),
            rect.size.x(),    rect.size.y());
    }
};

class Qt2Core {
public:

    static Vector2dd Vector2ddFromQPoint(const QPoint &point )
    {
        return Vector2dd(point.x(), point.y());
    }

    static Vector2dd Vector2ddFromQPointF(const QPointF &point )
    {
        return Vector2dd(point.x(), point.y());
    }

    static Vector2d32 Vector2d32FromQPoint(const QPoint &point )
    {
        return Vector2d32(point.x(), point.y());
    }

    static Rectangle32 RectangleFromQRect (const QRect &rect)
    {
        return Rectangle32(
            rect.left(),  rect.top(),
            rect.width(), rect.height());
    }
};

void drawLine(QPainter& painter, const Vector2dd &start, const Vector2dd &end);

void drawCircle(QPainter& painter, const Vector2dd &center, double radius);
void drawSquare(QPainter& painter, const Vector2dd &center, int size);

void draw2Circle (QPainter& painter,
                   const Vector2dd &center1, int size1,
                   const Vector2dd &center2, int size2
                   );


void drawMetaballs1(QPainter& painter,
                   const Vector2dd &center1, int size1,
                   const Vector2dd &center2, int size2
                   );


void drawMetaballs(QPainter& painter,
                   const Vector2dd &center1, int size1,
                   const Vector2dd &center2, int size2
                   );

void drawArc(QPainter& painter, const Vector2dd &center, double radius, int start, int len);

void drawPie(QPainter& painter, const Vector2dd &center, double radius, int start, int len);

void disableComboBoxItem(QComboBox *box, int item);

void uniteAnaglyph (const QImage *left, const QImage *right, QImage *result, uint32_t lmask = 0xFF00FF00, uint32_t rmask= 0xFFFF0000);

QRect fixedAspectRescaleRect (QRect source, QRect target);

#endif /* PAINTERHELPERS_H_ */
