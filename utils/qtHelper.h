#pragma once

/**
 * \file qtHelper.h
 *
 * \date Apr 27, 2013
 **/

#include <QtCore/QDebug>
#include <QDoubleSpinBox>

#include "vector2d.h"
#include "vector3d.h"

QDebug & operator<< (QDebug & stream, const corecvs::Vector2dd & vector);
QDebug & operator<< (QDebug & stream, const corecvs::Vector3dd & vector);

void setValueBlocking(QDoubleSpinBox *box, double value);

/* EOF */
