/**
 * \file sixDBuffer.cpp
 * \brief Add Comment Here
 *
 * \date Jan 8, 2012
 * \author alexander
 */

#include "utils/global.h"
#include "buffers/flow/sixDBuffer.h"

namespace corecvs {


SixDBuffer::SixDBuffer(const FloatFlowBuffer &flow, const FloatFlowBuffer &prevStereo,
                       const FloatFlowBuffer &stereo) :
        SixDBufferBase(flow.h, flow.w, false)
{
    for (int i = 0; i < flow.h; i++)
    {
        for (int j = 0; j < flow.w; j++)
        {
            Element6D element;
            bool hasFlow      = flow  .isElementKnown(i, j);
            bool hasStereo    = stereo.isElementKnown(i, j);
            bool hasOldStereo = false;

            if (hasFlow)
            {
                element.floatFlow = flow.element(i, j).vector;

                int oldX = j + element.flow.x();
                int oldY = i + element.flow.y();

                hasOldStereo = prevStereo.isElementKnown(oldY, oldX);
                if (hasOldStereo)
                {
                    element.dispPrev = prevStereo.element(oldY, oldX).vector.x();
                }
            }

            if (hasStereo) {
                element.disp = stereo.element(i, j).vector.x();
            }

            this->element(i, j) = element;
        }
    }
}

SixDBuffer::SixDBuffer(const FlowBuffer &flow, const FlowBuffer &prevStereo,
                       const FlowBuffer &stereo) :
        SixDBufferBase(flow.h, flow.w, false)
{
    for (int i = 0; i < flow.h; i++)
    {
        for (int j = 0; j < flow.w; j++)
        {
            Element6D element;
            bool hasFlow      = flow  .isElementKnown(i, j);
            bool hasStereo    = stereo.isElementKnown(i, j);
            bool hasOldStereo = false;

            if (hasFlow)
            {
                element.flow = flow.element(i, j);

                int oldX = j + element.flow.x();
                int oldY = i + element.flow.y();

                hasOldStereo = prevStereo.isElementKnown(oldY, oldX);
                if (hasOldStereo)
                {
                    element.dispPrev = prevStereo.element(oldY, oldX).x();
                }
            }

            if (hasStereo) {
                element.disp = stereo.element(i, j).x();
            }

            this->element(i, j) = element;
        }
    }
}


} // namespace corecvs
