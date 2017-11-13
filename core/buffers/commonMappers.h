#pragma once
/**
 * \file commonMappers.h
 * \brief a header for CommonMappers.cpp
 *
 *
 * \ingroup cppcorefiles
 * \date Feb 23, 2010
 * \author: alexander
 */
#include <functional>
#include <stdint.h>

#include "core/buffers/abstractBuffer.h"
#include "core/buffers/g12Buffer.h"

namespace corecvs {

using std::unary_function;

class GainOffsetMapper : public unary_function<uint16_t, uint16_t>
{
    double gain;
    int offset;

public:
    GainOffsetMapper(double _gain, int _offset)
    {
        gain   = _gain;
        offset = _offset;
    }

    uint16_t operator ()( const uint16_t &input) const
    {
        double value = ((double)input - offset) / gain;
        if (value < 0)
            return 0;
        if (value >= G12Buffer::BUFFER_MAX_VALUE)
            return G12Buffer::BUFFER_MAX_VALUE;

        return (uint16_t)value;
    }
};


class ShiftMaskMapper : public unary_function<uint16_t, uint16_t>
{
    uint32_t mask;
    int8_t   shift;

public:
    ShiftMaskMapper(uint32_t _mask, int8_t _shift)
    {
        mask  = _mask;
        shift = _shift;
    }

    uint16_t operator ()( const uint16_t &input) const
    {
        uint16_t value = input & mask;
        if (shift > 0)
            value <<= shift;
        if (shift < 0)
            value >>= (-shift);
        return value;
    }
};

class IntervalMapper : public unary_function<uint16_t, uint16_t>
{
    int32_t min;
    int32_t interval;

public:
    IntervalMapper(int32_t _min, int32_t _max)
    {
        min = _min;
        interval = _max - _min;
    }

    uint16_t operator ()( const uint16_t &input) const
    {
        int64_t value = ((uint64_t)input * interval) / G12Buffer::BUFFER_MAX_VALUE + min;
        if (value < 0)
            return 0;
        if (value > G12Buffer::BUFFER_MAX_VALUE)
            return G12Buffer::BUFFER_MAX_VALUE;

        return (uint16_t)value;
    }
};


} //namespace corecvs
