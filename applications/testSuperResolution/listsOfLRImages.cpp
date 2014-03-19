#include "listsOfLRImages.h"


LRImage::LRImage(int newNumberInImageCollection, double newShiftX, double newShiftY, double newAngleDegree, double newCoefficient)
{
    numberInImageCollection_ = newNumberInImageCollection;
    shiftX_ = newShiftX;
    shiftY_ = newShiftY;
    angleDegree_ = newAngleDegree;
    coefficient_ = newCoefficient;
}
