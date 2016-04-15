#pragma once

/**
 * \file qtHelper.h
 *
 * \date Apr 27, 2013
 **/

#include <QtCore/QDebug>
#include <QDoubleSpinBox>
#include <QItemSelectionModel>
#include <QTransform>
#include <QWidget>

#include "vector2d.h"
#include "vector3d.h"
#include "rectangle.h"
#include "matrix33.h"

using corecvs::Vector2d;
using corecvs::Vector2d32;
using corecvs::Vector2dd;
using corecvs::Rectangle32;
using corecvs::Vector3dd;
using corecvs::Matrix33;


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
            rect.size.  x(),  rect.size.  y());
    }

    static QTransform QTransformFromMatrix(const Matrix33 &m);
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


QDebug & operator<< (QDebug & stream, const Vector2dd & vector);
QDebug & operator<< (QDebug & stream, const Vector3dd & vector);

QDebug & operator<< (QDebug & stream, const Vector2d<int> & vector);


void setValueBlocking(QDoubleSpinBox *box, double value);

QString printWindowFlags(const Qt::WindowFlags &flags);
QString printWindowState(const Qt::WindowStates &state);
QString printSelecionModel(const QItemSelectionModel::SelectionFlags &flag);
QString printWidgetAttributes(QWidget *widget);





/* EOF */
