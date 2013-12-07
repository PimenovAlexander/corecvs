#include "resamples.h"
#include <cmath>
#include <iostream>

RGB24Buffer *resampleWithBilinearInterpolation(RGB24Buffer *startImage, double coefficient)
{
    RGB24Buffer *result = new RGB24Buffer((int)(startImage -> getH()*coefficient),(int)(startImage -> getW()*coefficient),false);
    for (int i = 0; i < result -> getH(); i++)
        for (int j = 0 ; j < result -> getW(); j++) {
            int xLeft = floor(((double)i) / coefficient);
            int yLeft = floor(((double)j) / coefficient);
            int xRight = min (xLeft+1,startImage -> getH()-1);
            int yRight = min (yLeft+1,startImage -> getW()-1);
            double k1 = ((double)i)/coefficient - (double)xLeft;
            double k2 = ((double)j)/coefficient - (double)yLeft;

            result -> element(i,j).r() =
                    startImage -> element(xLeft,yLeft).r() * k1 * k2 +
                    startImage -> element(xLeft,yRight).r() * k1 * (1 - k2) +
                    startImage -> element(xRight,yLeft).r() * (1 - k1) * k2 +
                    startImage -> element(xRight,yRight).r() * (1- k1) * (1 - k2);

            result -> element(i,j).g() =
                    startImage -> element(xLeft,yLeft).g() * k1 * k2 +
                    startImage -> element(xLeft,yRight).g() * k1 * (1 - k2) +
                    startImage -> element(xRight,yLeft).g() * (1 - k1) * k2 +
                    startImage -> element(xRight,yRight).g() * (1- k1) * (1 - k2);

            result -> element(i,j).b() =
                    startImage -> element(xLeft,yLeft).b() * k1 * k2 +
                    startImage -> element(xLeft,yRight).b() * k1 * (1 - k2) +
                    startImage -> element(xRight,yLeft).b() * (1 - k1) * k2 +
                    startImage -> element(xRight,yRight).b() * (1- k1) * (1 - k2);
        }
    return result;
}


uint8_t interpolate(uint8_t value0, uint8_t value1, uint8_t value2, uint8_t value3, double x)
{
    return value0 * (x-1)*(x-2)*(x-3)/(-6) +
            value1 * x * (x-2) * (x-3) / 2 +
            value2 * x * (x-1) * (x-3) / (-2) +
            value3 * x * (x-1) * (x-2) / 6;
}

RGB24Buffer *resampleWithBicubicInterpolation(RGB24Buffer *startImage, double coefficient)
{
    RGB24Buffer *result = new RGB24Buffer((int)(startImage -> getH()*coefficient),(int)(startImage -> getW()*coefficient),false);
    for (int i = 0; i < result -> getH(); i++)
        for (int j = 0 ; j < result -> getW(); j++)
        {
            int x0 = 4 * floor(((double)i) / (4 * coefficient));
            int y0 = 4 * floor(((double)j) / (4 * coefficient));
            if ((x0 + 3 < startImage -> getH()) && (y0 +3 < startImage -> getW()))
            {
                double dx = 4 * ((double)i)/(4 * coefficient) - (double)x0;
                double dy = 4 * ((double)j)/(4 * coefficient) - (double)y0;
                int x1 = x0 + 1;
                int x2 = x0 + 2;
                int x3 = x0 + 3;
                int y1 = y0 + 1;
                int y2 = y0 + 2;
                int y3 = y0 + 3;
                uint8_t tempResult0 = interpolate(startImage -> element(x0, y0).r(),
                                                  startImage -> element(x0, y1).r(),
                                                  startImage -> element(x0, y2).r(),
                                                  startImage -> element(x0, y3).r(),
                                                  dy);

                uint8_t tempResult1 = interpolate(startImage -> element(x1, y0).r(),
                                                  startImage -> element(x1, y1).r(),
                                                  startImage -> element(x1, y2).r(),
                                                  startImage -> element(x1, y3).r(),
                                                  dy);

                uint8_t tempResult2 = interpolate(startImage -> element(x2, y0).r(),
                                                  startImage -> element(x2, y1).r(),
                                                  startImage -> element(x2, y2).r(),
                                                  startImage -> element(x2, y3).r(),
                                                  dy);

                uint8_t tempResult3 = interpolate(startImage -> element(x3, y0).r(),
                                                  startImage -> element(x3, y1).r(),
                                                  startImage -> element(x3, y2).r(),
                                                  startImage -> element(x3, y3).r(),
                                                  dy);

                result -> element(i,j).r() = interpolate(tempResult0,tempResult1,tempResult2,tempResult3,dx);

                tempResult0 = interpolate(startImage -> element(x0, y0).g(),
                                          startImage -> element(x0, y1).g(),
                                          startImage -> element(x0, y2).g(),
                                          startImage -> element(x0, y3).g(),
                                                  dy);

                tempResult1 = interpolate(startImage -> element(x1, y0).g(),
                                          startImage -> element(x1, y1).g(),
                                          startImage -> element(x1, y2).g(),
                                          startImage -> element(x1, y3).g(),
                                                  dy);

                tempResult2 = interpolate(startImage -> element(x2, y0).g(),
                                          startImage -> element(x2, y1).g(),
                                          startImage -> element(x2, y2).g(),
                                          startImage -> element(x2, y3).g(),
                                                  dy);

                tempResult3 = interpolate(startImage -> element(x3, y0).g(),
                                          startImage -> element(x3, y1).g(),
                                          startImage -> element(x3, y2).g(),
                                          startImage -> element(x3, y3).g(),
                                                  dy);

                result -> element(i,j).g() = interpolate(tempResult0,tempResult1,tempResult2,tempResult3,dx);

                tempResult0 = interpolate(startImage -> element(x0, y0).b(),
                                          startImage -> element(x0, y1).b(),
                                          startImage -> element(x0, y2).b(),
                                          startImage -> element(x0, y3).b(),
                                                  dy);

                tempResult1 = interpolate(startImage -> element(x1, y0).b(),
                                          startImage -> element(x1, y1).b(),
                                          startImage -> element(x1, y2).b(),
                                          startImage -> element(x1, y3).b(),
                                                  dy);

                tempResult2 = interpolate(startImage -> element(x2, y0).b(),
                                          startImage -> element(x2, y1).b(),
                                          startImage -> element(x2, y2).b(),
                                          startImage -> element(x2, y3).b(),
                                                  dy);

                tempResult3 = interpolate(startImage -> element(x3, y0).b(),
                                          startImage -> element(x3, y1).b(),
                                          startImage -> element(x3, y2).b(),
                                          startImage -> element(x3, y3).b(),
                                                  dy);

                result -> element(i,j).b() = interpolate(tempResult0,tempResult1,tempResult2,tempResult3,dx);
            }
        }
    return result;
}

