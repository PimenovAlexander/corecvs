#ifndef SIXDBUFFER_H_
#define SIXDBUFFER_H_
/**
 * \file sixDBuffer.h
 * \brief A header for sixDBuffer.cpp
 *
 *  This file holds the SixDBuffer class which in it's turn describe the knowledge
 *  of the 3D point positions and speeds in 3D scene.
 *
 * \date Jan 8, 2012
 * \author alexander
 * \ingroup cppcorefiles
 */

#include <stdint.h>

#include "core/utils/global.h"

#include "core/buffers/abstractContiniousBuffer.h"
#include "core/buffers/flow/element6D.h"
#include "core/buffers/flow/punchedBufferOperations.h"
#include "core/buffers/flow/floatFlowBuffer.h"

namespace corecvs {


typedef AbstractContiniousBuffer< Element6D , int32_t> SixDBufferBase;

/**
 *   6D buffer that hold all the information extracted form the images
 **/
class SixDBuffer :
    public SixDBufferBase,
    public PunchedBufferOperations<SixDBuffer, SixDBufferBase::InternalElementType>
{
public:

    SixDBuffer() : SixDBufferBase() {}

    SixDBuffer(int32_t h, int32_t w) : SixDBufferBase(h, w, Element6D()) {}

    SixDBuffer(Vector2d<int32_t> size) : SixDBufferBase(size, Element6D()) {}

    SixDBuffer(SixDBuffer &that) : SixDBufferBase(that) {}

    SixDBuffer(SixDBuffer *that) : SixDBufferBase (that) {}

    SixDBuffer(SixDBuffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
        : SixDBufferBase(src, x1, y1, x2, y2) {}

    SixDBuffer(uint32_t h, uint32_t w, bool shouldClear) : SixDBufferBase(h, w, shouldClear) {}

    SixDBuffer(uint32_t h, uint32_t w, FlowElement *data) : SixDBufferBase(h, w, data) {}

    ~SixDBuffer() {}

    inline bool isElementKnown(const int32_t y, const int32_t x) const
    {
        return this->element(y, x).is6D();
    }

    inline void setElementUnknown(const int32_t y, const int32_t x)
    {
        element(y, x) = Element6D();
    }

    SixDBuffer(const FlowBuffer &flow, const FlowBuffer &prevStereo, const FlowBuffer &stereo);

    SixDBuffer(const FloatFlowBuffer &flow, const FloatFlowBuffer &prevStereo, const FloatFlowBuffer &stereo);

};


} // namespace corecvs

#endif  /* #ifndef SIXDBUFFER_H_ */


