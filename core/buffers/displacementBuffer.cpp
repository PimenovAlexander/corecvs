/**
 * \file displacementBuffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 21, 2010
 * \author alexander
 */

#include "displacementBuffer.h"
namespace corecvs {


DisplacementBuffer *DisplacementBuffer::CacheInverse(
    RadialCorrection *inverseMap,
    int h, int w,
    double x1, double y1,
    double x2, double y2,
    double step,
    bool useLM
)
{
    DisplacementBuffer *toReturn = new DisplacementBuffer(h, w);
    AbstractBuffer<double> *distance = new AbstractBuffer<double>(h, w, 10.0);

    for (double y = y1; y < y2; y += step)
    {
        for (double x = x1; x < x2; x += step)
        {
            Vector2dd image = inverseMap->map(y,x);
            int imageX = fround(image.x());
            int imageY = fround(image.y());
            if (!toReturn->isValidCoord(imageY, imageX)) {
                continue;
            }
            double olddist = distance->element(imageY, imageX);
            double dist = (Vector2dd(imageX, imageY) - image).l2Metric();

            if (olddist > dist) {
                toReturn->element(imageY, imageX) = Vector2dd(x,y) - Vector2dd(imageX, imageY);
                distance->element(imageY, imageX) = dist;
            }
        }
    }

    if (useLM)
    {
        LevenbergMarquardt levMarq(10, 2, 1.5);
        levMarq.f = inverseMap;
        for (int i = 0; i < toReturn->h; i ++)
        {
            for (int j = 0; j < toReturn->w; j ++)
            {
                Vector2dd guessV = toReturn->element(i, j) + Vector2dd(j, i);
                vector<double> guess(2);
                guess[0] = guessV.x();
                guess[1] = guessV.y();
                vector<double> target(2);
                target[0] = j;
                target[1] = i;
                vector<double> improved = levMarq.fit(guess, target);
                toReturn->element(i, j) = Vector2dd(improved[0],improved[1]) - Vector2dd(j, i);
            }
        }
    }

    delete_safe(distance);
    return toReturn;
}

} //namespace corecvs

