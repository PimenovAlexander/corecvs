#ifndef PAINTERHELPERS_H_
#define PAINTERHELPERS_H_
/**
 * \file painterHelpers.h
 * \brief Add Comment Here
 *
 * \date Nov 5, 2011
 * \author alexander
 */


#include <stdint.h>
#include <QtCore/QRect>
#include <QPainter>
#include <QComboBox>

#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"

using namespace corecvs;

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

QImage *checkerBoard(int h, int w, int square);

#endif /* PAINTERHELPERS_H_ */
