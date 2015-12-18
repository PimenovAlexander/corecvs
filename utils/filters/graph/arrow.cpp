/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "arrow.h"
#include <math.h>
#include "iostream"
using namespace std;

const qreal Pi = 3.14;

//! [0]
Arrow::Arrow(DiagramItem *startItem, DiagramItem *endItem,
         QGraphicsItem *parent, QGraphicsScene * /*scene*/)
    : QGraphicsLineItem(parent/*, scene*/)
{
//    cout<<"Arrow::Arrow"<<endl;

    myStartItem = startItem;
    myEndItem   = endItem;

    setFlag(QGraphicsItem::ItemIsSelectable, true);
    myColor = Qt::black;
    setPen(QPen(myColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}
//! [0]

//! [1]
QRectF Arrow::boundingRect() const
{
//    cout<<"Arrow::boundingRect()"<<endl;
    qreal extra = (pen().width() + 20) / 2.0;

    return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                      line().p2().y() - line().p1().y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}
//! [1]

//! [2]
QPainterPath Arrow::shape() const
{
//    cout<<"Arrow::shape()"<<endl;
    QPainterPath path = QGraphicsLineItem::shape();
    path.addPolygon(arrowHead);
    return path;
}
//! [2]

//! [3]
void Arrow::updatePosition()
{
//    cout<<"Arrow::updatePosition()"<<endl;
//    QLineF line(mapFromItem(myStartItem, 0, 0), mapFromItem(myEndItem, 0, 0));
//    setLine(line);?????
//    cout<<"line "<<line.x1()<<" "<<line.y1()<<" "<<line.x2()<<" "<<line.y2()<<endl;
}
//! [3]

//! [4]
void Arrow::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
          QWidget *)
{

    QColor currentColor;
    if (isSelected())
        currentColor = Qt::red;
    else
        currentColor = myColor;


    if (myStartItem->collidesWithItem(myEndItem))
    {
        cout<<"myStartItem->collidesWithItem(myEndItem)"<<endl;
//        cout<<"paint line return "<<line().x1()<<" "<<line().y1()<<" "<<line().x2()<<" "<<line().y2()<<endl;
        return;
    }

    QPen myPen = pen();
    myPen.setColor(currentColor);
    qreal arrowSize = 20;
    painter->setPen(myPen);
    painter->setBrush(currentColor);

    QPointF startPos = mapFromItem(myStartItem, 0, 0);
    QPointF endPos   = mapFromItem(myEndItem,   0, 0);
    QLineF centerLine(startPos, endPos);
//    cout<<"startPos = "<<startPos.x()<<" "<<startPos.y()<< endl;
//    cout<<"endPos   = "<<endPos.x()  <<" "<<endPos.y()  << endl;


    QPolygonF endPolygon = myEndItem->polygon();
    endPolygon = mapFromItem(myEndItem, myEndItem->polygon());
    QPointF p1 = endPolygon.first();
//    cout<<"p1 = "<<p1.x()<<" "<<p1.y()<<endl;
    QPointF p2;
    QPointF intersectPoint;
    for (int i = 1; i < endPolygon.count(); ++i) {
        p2 = endPolygon.at(i);
//        cout<<"p2 = "<<p2.x()<<" "<<p2.y()<<endl;
        QLineF polyLine = QLineF(p1, p2);
        QLineF::IntersectType intersectType =
        polyLine.intersect(centerLine, &intersectPoint);
        if (intersectType == QLineF::BoundedIntersection)
            break;
        p1 = p2;
    }

    setLine(QLineF(intersectPoint, startPos));
//    cout<<"paint line setLine "<<line().x1()<<" "<<line().y1()<<" "<<line().x2()<<" "<<line().y2()<<endl;

    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

    QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                    cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                    cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p1() << arrowP1 << arrowP2;

    painter->drawLine(line());
    painter->drawPolygon(arrowHead);
    if (isSelected()) {
        painter->setPen(QPen(currentColor, 1, Qt::DashLine));
        QLineF myLine = line();
        myLine.translate(0, 4.0);
        painter->drawLine(myLine);
        myLine.translate(0,-8.0);
        painter->drawLine(myLine);
    }
}
