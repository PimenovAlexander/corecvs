#include "transformations.h"
#include <cmath>
#define M
RGB24Buffer *rotate(RGB24Buffer *startImage, double angleDegree)
{
    double angle = M_PI * angleDegree / 180;
    RGB24Buffer *result = new RGB24Buffer(startImage -> getH(),startImage -> getW(),false);
    for (int i = 0; i < result -> getH(); i++)
        for (int j = 0; j < result -> getW(); j++)
        {
            result -> element(i, j) = RGBColor(0,0,0);
        }
    for (int i = 0; i < result -> getH(); i++)
        for (int j = 0; j < result -> getW(); j++)
        {
            int resX = i - startImage -> getH()/2;
            int resY = j - startImage -> getW()/2;

            int tempX = resX;
            int tempY = resY;

            resX = floor((double)tempX - (double)tempY*tan(angle/2));
            resY = tempY;
            //resX = startImage -> getH()/2 + floor((double)tempX * cos(-angle) + (double)tempY*sin(-angle));
            //resY = startImage -> getW()/2 + floor(-(double)tempX * sin(-angle) + (double)tempY*cos(-angle));

            tempX = resX;
            tempY = resY;

            resX = tempX;
            resY = floor((double)tempY + (double)tempX*sin(angle));


            tempX = resX;
            tempY = resY;

            resX = startImage -> getH()/2 + floor((double)tempX - (double)tempY*tan(angle/2));
            resY = startImage -> getW()/2 + tempY;
            if ((resX >= 0) && (resX <result -> getH()) && (resY >= 0) && (resY < result -> getW()))
            {
                result -> element(resX, resY) = startImage -> element(i,j);
            }
        }

    return result;
}
