/**
 * \file displacementBuffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 21, 2010
 * \author alexander
 */
#include "core/buffers/displacementBuffer.h"

namespace corecvs {


/**
 *   This method  tries to invert the radial correction buffer there are different ways to do this in terms of speed/quality
 *
 **/
void DisplacementBuffer::fillWithInvertedRadialBase(RadialCorrection *inverseMap, double x1, double y1, double x2, double y2, double step, bool useLM)
{
    AbstractBuffer<double> *distance = new AbstractBuffer<double>(h, w, 10.0);

    for (double y = y1; y < y2; y += step)
    {
        for (double x = x1; x < x2; x += step)
        {
            Vector2dd image = inverseMap->map(y,x);
            int imageX = fround(image.x());
            int imageY = fround(image.y());
            if (!isValidCoord(imageY, imageX)) {
                continue;
            }
            double olddist = distance->element(imageY, imageX);
            double dist = (Vector2dd(imageX, imageY) - image).l2Metric();

            if (olddist > dist) {
                element(imageY, imageX) = Vector2dd(x,y) - Vector2dd(imageX, imageY);
                distance->element(imageY, imageX) = dist;
            }
        }
    }

    if (useLM)
    {
        LevenbergMarquardt levMarq(10, 2, 1.5);
        levMarq.trace = false;
        levMarq.traceProgress = false;
        levMarq.f = inverseMap;

        for (int i = 0; i < h; i ++)
        {
            for (int j = 0; j < w; j ++)
            {
                Vector2dd guessV = element(i, j) + Vector2dd(j, i);
                vector<double> guess(2);
                guess[0] = guessV.x();
                guess[1] = guessV.y();
                vector<double> target(2);
                target[0] = j;
                target[1] = i;
                vector<double> improved = levMarq.fit(guess, target);
                element(i, j) = Vector2dd(improved[0],improved[1]) - Vector2dd(j, i);
            }
        }
    }

    delete_safe(distance);
}


DisplacementBuffer *DisplacementBuffer::CacheInverse(
    RadialCorrection *inverseMap,
    int h, int w,
    double x1, double y1,
    double x2, double y2,
    double step,
    bool useLM
)
{
    cout << "DisplacementBuffer::CacheInverse() called" << endl;
    cout << "  Inverse Map:" << inverseMap->mParams << endl;
    cout << "  H:" << h << " W:" << w << endl;
    cout << "  x1:" << x1 << " y1:" << y1 << endl;
    cout << "  x2:" << x2 << " y2:" << y2 << endl;
    cout << "  step:" << step << endl;
    cout << "  UseLM:" << (useLM ? "yes" : "no") << endl;

    DisplacementBuffer *toReturn = new DisplacementBuffer(h, w, Vector2dd(numeric_limits<double>::max(), numeric_limits<double>::max()));
    toReturn->fillWithInvertedRadialBase(inverseMap, x1, y1, x2, y2, step, useLM);

    return toReturn;
}

DisplacementBuffer *DisplacementBuffer::TestWiggle(int h, int w, double power, double step)
{
    DisplacementBuffer *toReturn = new DisplacementBuffer(h, w);

    for (int i = 0; i < toReturn->h; i++ )
    {
        for (int j = 0; j < toReturn->w; j++ )
        {
            toReturn->element(i,j) =  Vector2dd(sin(i / step), sin(j / step)) * power ;
            /*if ((i / 100) % 2) {
                toReturn->element(i,j) = Vector2dd(1.4, 0);
            } else {
                toReturn->element(i,j) = Vector2dd(1, 0);
            }*/
        }
    }

    return toReturn;
}

TableInverseCache *TableInverseCache::CacheInverse(int tx1, int ty1, int tx2, int ty2, RadialCorrection *inverseMap, double x1, double y1, double x2, double y2, double step, bool useLM)
{
    cout << "DisplacementBuffer::CacheInverse() called" << endl;
    cout << "  Inverse Map:" << inverseMap->mParams << endl;
    cout << "  We would map: " << endl;
    cout << "    X1:" << tx1 << " Y1:" << ty1 << endl;
    cout << "    X2:" << tx2 << " Y2:" << ty2 << endl;
    cout << "  To: " << endl;
    cout << "    x1:" << x1 << " y1:" << y1 << endl;
    cout << "    x2:" << x2 << " y2:" << y2 << endl;
    cout << "  step:" << step << endl;
    cout << "  UseLM:" << (useLM ? "yes" : "no") << endl;



    RadialCorrection input = *inverseMap;
    TableInverseCache *toReturn = new TableInverseCache(ty2 - ty1, tx2 - tx1);

    toReturn->inputShift = Vector2dd(tx1, ty1);
    input.mParams.setShift(input.mParams.shift() - Vector2dd(tx1, ty1));
    toReturn->fillWithInvertedRadialBase(&input, x1, y1, x2, y2, step, useLM);
    return toReturn;
}

} //namespace corecvs
