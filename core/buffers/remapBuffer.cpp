#include "remapBuffer.h"

/**
 * \file displacementBuffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 18, 2018
 */
#include "buffers/remapBuffer.h"

namespace corecvs {


/**
 *   This method  tries to invert the radial correction buffer there are different ways to do this in terms of speed/quality
 *
 **/
void RemapBuffer::fillWithInvertedRadialBase(RadialCorrection *inverseMap, double x1, double y1, double x2, double y2, double step, bool useLM)
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
                element(imageY, imageX) = Vector2dd(x,y);
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
                Vector2dd guessV = element(i, j);
                vector<double> guess(2);
                guess[0] = guessV.x();
                guess[1] = guessV.y();
                vector<double> target(2);
                target[0] = j;
                target[1] = i;
                vector<double> improved = levMarq.fit(guess, target);
                element(i, j) = Vector2dd(improved[0],improved[1]);
            }
        }
    }

    delete_safe(distance);
}


RemapBuffer *RemapBuffer::CacheInverse(
    RadialCorrection *inverseMap,
    int h, int w,
    double x1, double y1,
    double x2, double y2,
    double step,
    bool useLM
)
{
    cout << "RemapBuffer::CacheInverse() called" << endl;
    cout << "  Inverse Map:" << inverseMap->mParams << endl;
    cout << "  H:" << h << " W:" << w << endl;
    cout << "  x1:" << x1 << " y1:" << y1 << endl;
    cout << "  x2:" << x2 << " y2:" << y2 << endl;
    cout << "  step:" << step << endl;
    cout << "  UseLM:" << (useLM ? "yes" : "no") << endl;

    RemapBuffer *toReturn = new RemapBuffer(h, w, Vector2dd(numeric_limits<double>::max(), numeric_limits<double>::max()));
    toReturn->fillWithInvertedRadialBase(inverseMap, x1, y1, x2, y2, step, useLM);

    return toReturn;
}

RemapBuffer *RemapBuffer::TestWiggle(int h, int w, double power, double step)
{
    RemapBuffer *toReturn = new RemapBuffer(h, w);

    for (int i = 0; i < toReturn->h; i++ )
    {
        for (int j = 0; j < toReturn->w; j++ )
        {
            toReturn->element(i,j) =  Vector2dd(sin(i / step), sin(j / step)) * power + Vector2dd(j, i);
        }
    }

    return toReturn;
}

} //namespace corecvs
