#pragma once
/**
 * \file disparityBuffer.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <stdint.h>
#include "buffers/abstractContiniousBuffer.h"

namespace corecvs {

#ifndef INT16_MAX
#define INT16_MAX (0x7FFF)
#endif

typedef uint16_t DisparityElement;
typedef AbstractContiniousBuffer<DisparityElement, int32_t> DisparityBufferBase;

class DisparityBuffer : public DisparityBufferBase
{
public:
    static const int16_t DISPARITY_UNKNOWN = INT16_MAX;

    int32_t maxValue;

    DisparityBuffer(int32_t h, int32_t w) :
        DisparityBufferBase(h, w, (DisparityElement)DisparityBuffer::DISPARITY_UNKNOWN) {}

    DisparityBuffer(DisparityBuffer &that) : DisparityBufferBase (that) {}
    DisparityBuffer(DisparityBuffer *that) : DisparityBufferBase (that) {}

    DisparityBuffer(DisparityBuffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        DisparityBufferBase(src, x1, y1, x2, y2) {}

    DisparityBuffer(uint32_t h, uint32_t w, bool shouldClear) :
        DisparityBufferBase(h, w, shouldClear) {}

    DisparityBuffer(uint32_t h, uint32_t w, DisparityElement *data) :
        DisparityBufferBase(h, w, data) {}
};

} //namespace corecvs
