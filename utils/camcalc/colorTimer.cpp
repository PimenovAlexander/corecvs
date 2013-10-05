#include "colorTimer.h"

ColorTimer::ColorTimer(QDoubleSpinBox *box) :
    QTimer(box),
    mBox(box)
{
    mTimeoutNum = 0;
    connect(this, SIGNAL(timeout()), this, SLOT(emitSignal()));
}

ColorTimer::~ColorTimer()
{
    mBox = NULL;
}

void ColorTimer::emitSignal()
{
    mTimeoutNum ++;
    emit colorTimeout(mBox, mTimeoutNum);
}
