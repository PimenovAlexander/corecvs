#pragma once
#include "rgb24Buffer.h"

RGB24Buffer *resampleWithBilinearInterpolation(RGB24Buffer *startImage, double coefficient);

RGB24Buffer *resampleWithLancsozFilter(RGB24Buffer *startImage, double coefficient, int a);

RGB24Buffer *resampleWithNearestNeighbour(RGB24Buffer *startImage, double coefficient);

RGB24Buffer *squareBasedResampling(RGB24Buffer *startImage, double coefficient, double shiftX, double shiftY, double angleDegree);

double getIntersectionOfSquares(double A_x, double A_y, double B_x, double B_y,double C_x, double C_y,double D_x, double D_y, int startPoint_x, int startPoint_y);
