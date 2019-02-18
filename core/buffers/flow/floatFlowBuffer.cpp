/**
 * \file floatFlowBuffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 20, 2010
 * \author alexander
 */
#include "core/utils/global.h"

#include "core/buffers/flow/floatFlowBuffer.h"
#include "core/stats/calculationStats.h"

namespace corecvs {

FloatFlowBuffer::FloatFlowBuffer(FlowBuffer *that)
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

FlowBuffer *FloatFlowBuffer::toFlowBuffer()
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

} //namespace corecvs

