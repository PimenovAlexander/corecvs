#pragma once
#include "rgb24Buffer.h"

RGB24Buffer *resampleWithBilinearInterpolation(RGB24Buffer *startImage, double coefficient);

RGB24Buffer *resampleWithBicubicInterpolation(RGB24Buffer *startImage, double coefficient);
