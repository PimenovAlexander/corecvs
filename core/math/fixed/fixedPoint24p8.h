#ifndef FIXED_POINT24P8_H_
#define FIXED_POINT24P8_H_

/**
 * \file fixedPoint24p8.h
 * \brief Add Comment Here
 *
 * \date Mar 1, 2012
 * \author alexander
 */

#include <stdint.h>


namespace corecvs
{

class FixedPoint24p8
{
public:
    static const unsigned SHIFT_BITS = 8;
    static const unsigned FIXED_SCALE = 256;

    uint32_t data;

    FixedPoint24p8();

    explicit FixedPoint24p8(const uint32_t &_data)
    {
        data = _data;
    }

    explicit FixedPoint24p8(const double &value)
    {
        data = fround(value * FIXED_SCALE);
    }

    explicit FixedPoint24p8(const float &value)
    {
        data = fround(value * FIXED_SCALE);
    }

    FixedPoint24p8 operator + (const FixedPoint24p8 &arg1, const FixedPoint24p8 &arg2)
    {
        return FixedPoint24p8(arg1.data + arg2.data);
    }

    FixedPoint24p8 operator - (const FixedPoint24p8 &arg1, const FixedPoint24p8 &arg2)
    {
        return FixedPoint24p8(arg1.data - arg2.data);
    }

    FixedPoint24p8 operator * (const FixedPoint24p8 &arg1, const FixedPoint24p8 &arg2)
    {
        uint64_t result = arg1.data * arg2.data;
        return result >> SHIFT_BITS;
    }

    FixedPoint24p8 mulFast (const FixedPoint24p8 &arg1, const FixedPoint24p8 &arg2)
    {
        return (arg1.data * arg2.data) >> SHIFT_BITS;
    }

    FixedPoint24p8 fromUInt(uint32_t value)
    {
        return value << SHIFT_BITS;
    }

};

} /* namespace corecvs */
#endif /* FIXED_POINT24P8_H_ */
