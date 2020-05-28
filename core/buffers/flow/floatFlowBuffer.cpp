/**
 * \file floatFlowBuffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 20, 2010
 * \author alexander
 */
#include "utils/global.h"

#include "buffers/flow/floatFlowBuffer.h"
#include "stats/calculationStats.h"

namespace corecvs {

FloatFlowBuffer::FloatFlowBuffer(const FlowBuffer *that)
    : FloatFlowBufferBase(that->h, that->w, true)
{
    for (int i = 0; i < that->h; i++)
    {
        for (int j = 0; j < that->w; j++)
        {
            if (!that->isElementKnown(i, j))
                continue;

            FlowElement shift = that->element(i, j);
            this->element(i, j) = FloatFlow(Vector2dd(shift.x(), shift.y()));
        }
    }
}

FlowBuffer *FloatFlowBuffer::toFlowBuffer() const
{
    FlowBuffer *toReturn = new FlowBuffer(h, w);
    for (int i = 0; i < this->h; i++)
    {
        for (int j = 0; j < this->w; j++)
        {
            if (this->isElementKnown(i, j)) {
                FloatFlow shift = this->element(i, j);
                toReturn->element(i, j) = FlowElement(fround(shift.vector.x()), fround(shift.vector.y()));
            } else {
                toReturn->setElementUnknown(i,j);
            }
        }
    }
    return toReturn;
}


/**
 * Kitti : For this benchmark, we consider a pixel to be correctly estimated if the disparity or flow end-point error is <3px or <5%
 * (for scene flow this criterion needs to be fulfilled for both disparity maps and the flow map).
 **/
FloatFlowQualityData FloatFlowBuffer::compare(FloatFlowBuffer &groundTruth, double percentTreshold, double distanceTreshold)
{
    FloatFlowQualityData toReturn;
    toReturn.density = density();
    toReturn.gtDensity = groundTruth.density();

    int bothCount = 0;
    int distPass = 0;
    int percPass = 0;
    int pass = 0;
    int fesum = 0;

    for (int i = 0; i < groundTruth.h; i++)
    {
        for (int j = 0; j < groundTruth.w; j++)
        {
            if (!groundTruth.isElementKnown(i,j))
                continue;

            if (!isElementKnown(i,j))
                continue;

            bothCount++;

            Vector2dd gtFlow  = groundTruth.element(i,j).vector;
            Vector2dd curFlow = element(i,j).vector;

            bool okD = false, okP = false;
            Vector2dd delta = curFlow - gtFlow;
            fesum += delta.l2Metric();

            //cout << "[" << i << "," << j << "] - " << delta << endl;

            if (delta.l2Metric() < distanceTreshold) {
                okD = true;
                distPass++;
            }

            if (delta.l2Metric() / gtFlow.l2Metric() < percentTreshold / 100.0) {
                okP = true;
                percPass++;
            }

            if (okD || okP) {
                pass++;
            }

        }
    }
    toReturn.Finliers = ((double)pass / bothCount) * 100;
    toReturn.percent   = ((double)bothCount / toReturn.gtDensity) * 100;
    toReturn.afe       = ((double)fesum / bothCount);
    return toReturn;
}

double FloatFlowBuffer::getMaximumMagnitude() const
{
    double maximum = 0;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (!isElementKnown(i,j))
            {
                continue;
            }
            maximum = std::max(element(i,j).vector.l2Metric(), maximum);
        }
    }
    return maximum;
}

void FloatFlowQualityData::addToStats(Statistics *stats)
{
    if (stats == NULL) {
        return;
    }
    stats->setValue("gtDensity", gtDensity);
    stats->setValue("density", density);
    stats->setValue("precent", percent);
    stats->setValue("Finliers", Finliers);
    stats->setValue("afe", afe);
}

void corecvs::FloatFlowBuffer::kittiBackgroudFill()
{
    for (int v = 0; v < h; v++)
    {
        int count = 0;
        for (int u = 0; u < w; u++)
        {
            if (!isElementKnown(v, u) || count == 0)
            {
                count++;
                continue;
            }

            // first and last value for interpolation
            int32_t u1 = u - count;
            int32_t u2 = u - 1;

            // set pixel to min flow
            if (u1 > 0 && u2 < w - 1)
            {
                FloatFlow f1 = element(v, u1 - 1);
                FloatFlow f2 = element(v, u2 + 1);

                float fu_ipol = std::min(f1.vector.x(), f2.vector.x());
                float fv_ipol = std::min(f1.vector.y(), f2.vector.y());
                for (int32_t u_curr = u1; u_curr <= u2; u_curr++) {
                    element(v, u_curr) = FloatFlow(fu_ipol, fv_ipol);
                }
            }
            count = 0;
        }

        // extrapolate to the left
        for (int32_t u = 0; u < w; u++)
        {
            if (!isElementKnown(v, u)) {
                continue;
            }
            for (int32_t u2 = 0; u2 < u; u2++) {
                element(v, u2) = element(v,u);
            }
            break;
        }

        // extrapolate to the right
        for (int32_t u = w - 1; u >= 0; u--)
        {
            if (!isElementKnown(v, u)) {
                continue;
            }
            for (int32_t u2 = u + 1; u2 <= w - 1; u2++) {
                element(v, u2) = element(v,u);
            }
            break;
        }
    }

    // for each column do extrapolate to the top
    for (int32_t u = 0; u < w; u++)
    {
        for (int32_t v = 0; v < h; v++)
        {
            if (!isElementKnown(v, u)) {
                continue;
            }
            for (int32_t v2 = 0; v2 < v; v2++) {
                element(v2, u) = element(v,u);
            }
            break;
        }

        // extrapolate to the bottom
        for (int32_t v = h - 1; v >= 0; v--)
        {
            if (!isElementKnown(v, u)) {
                continue;
            }
            for (int32_t v2 = v + 1; v2 <= h - 1; v2++) {
                element(v2, u) = element(v,u);
            }
            break;
        }

    }

}

HSVColor FloatFlow::getHueColor(double maxMag, double scaler, double multiplier)
{
    if (!isKnown) {
          return HSVColor::Black();
    }

    double mag = vector.l2Metric();
    mag = std::max(mag, maxMag);
    double dir = vector.argument();

    uint16_t h = ((int)(radToDeg(dir) + 360.0)) % 360;
    double ds = clamp(mag * multiplier / scaler, 0.0, 1.0);
    double dv = clamp(          multiplier - ds, 0.0, 1.0);
    uint8_t  s = clamp((int)(ds * 255), 0,255);
    uint8_t  v = clamp((int)(dv * 255), 0,255);
    return HSVColor(h,s,v);
}

RGBColor FloatFlow::getColor(double maxMag, double scaler, double multiplier, bool kitti)
{
    return RGBColor::FromHSV(getHueColor(maxMag, scaler, multiplier), kitti);
}

RGBColor FloatFlow::getKittiColor(double maxMag, double scaler, double multiplier)
{
    return RGBColor::FromHSVKitti(getHueColor(maxMag, scaler, multiplier));
}

} //namespace corecvs

