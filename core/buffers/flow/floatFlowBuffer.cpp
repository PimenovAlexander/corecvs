/**
 * \file floatFlowBuffer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jul 20, 2010
 * \author alexander
 */
#include "global.h"

#include "floatFlowBuffer.h"

namespace corecvs {

FloatFlowBuffer::FloatFlowBuffer(FlowBuffer *that) :
        FloatFlowBufferBase(that->h, that->w, true)
{
    for (int i = 0; i < that->h; i++)
    {
        for (int j = 0; j < that->w; j++)
        {
            if (!that->isElementKnown(i,j))
                continue;

            FlowElement shift = that->element(i,j);
            FloatFlow floatFlow = FloatFlow(Vector2dd(shift.x(), shift.y()));
            this->element(i,j) = floatFlow;
        }
    }
}

} //namespace corecvs

