#pragma once
#include "rgb24Buffer.h"

RGB24Buffer *resampleWithBilinearInterpolation(RGB24Buffer *startImage, double coefficient);

RGB24Buffer *resampleWithLancsozFilter(RGB24Buffer *startImage, double coefficient, int a);

RGB24Buffer *resampleWithNearestNeighbour(RGB24Buffer *startImage, double coefficient);

RGB24Buffer *squareBasedResampling(RGB24Buffer *startImage, double coefficient, int shiftX, int shiftY);
