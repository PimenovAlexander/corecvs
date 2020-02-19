#pragma once
/**
 * \file depthBuffer.h
 *
 * \date Dec 6, 2012
 **/
#include <limits>

#include "utils/global.h"

#include "buffers/abstractContiniousBuffer.h"
#include "buffers/flow/punchedBufferOperations.h"

namespace corecvs
{

//typedef AbstractContiniousBuffer<double, int32_t> DepthBufferBase;

class DepthBuffer :
    public DpImage,
    public PunchedBufferOperations<DepthBuffer, DpImage::InternalElementType>
{
public:
    static const double DEPTH_UNKNOWN /*= -1.0*/;   // msvc requires moved this to cpp

    DepthBuffer(int32_t h, int32_t w) :
        DpImage ( h, w, DEPTH_UNKNOWN)
    {}

    DepthBuffer(Vector2d<int32_t> size) :
        DpImage ( size, DEPTH_UNKNOWN)
    {}


    DepthBuffer(DepthBuffer &that) : DpImage (that) {}
    DepthBuffer(DepthBuffer *that) : DpImage (that) {}

    DepthBuffer(DepthBuffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        DpImage(src, x1, y1, x2, y2) {}

    DepthBuffer(uint32_t h, uint32_t w, bool shouldClear) : DpImage(h, w, shouldClear) {}

    DepthBuffer(uint32_t h, uint32_t w, double *data) : DpImage(h, w, data) {}


    DepthBuffer();

    inline bool isElementKnown(const int32_t y, const int32_t x) const
    {
        return this->element(y, x) != DEPTH_UNKNOWN;
    }

    inline void setElementUnknown(const int32_t y, const int32_t x)
    {
        this->element(y, x) = DEPTH_UNKNOWN;
    }

   ~DepthBuffer();
};

} /* namespace corecvs */

/* EOF */
