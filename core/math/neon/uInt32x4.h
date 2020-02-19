#ifndef UINT32X4_H_
#define UINT32X4_H_
/**
 * \file uint32x4.h
 * \brief a header for UInt32x4.cpp
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include <arm_neon.h>
#include <stdint.h>

#include "utils/global.h"
#include "math/vector/fixedVector.h"
#include "math/sse/int64x2.h"
namespace corecvs {

class ALIGN_DATA(16) UInt32x4
{
public:
    /**
     *  Handle to SSE 128 value.
     *
     *  NB Union here dramatically decrease speed. So don't use one
     **/

    static const int SIZE = 4;


    uint32x4_t data;

    /* Constructors */
    UInt32x4(){}
    /**
     *  Copy constructor
     **/
    UInt32x4(const UInt32x4 &other) {
        this->data = other.data;
    }

    /**
     *  Create SSE integer vector from integer constant
     **/
    UInt32x4(const uint32x4_t &_data) {
        this->data = _data;
    }

    UInt32x4(const int16x8_t &_data) {
        this->data = (uint32x4_t)_data;
    }

    UInt32x4(const int32x4_t &_data) {
        this->data = (uint32x4_t)_data;
    }

    UInt32x4(uint32_t constant) {
        this->data = vdupq_n_u32(constant);
    }

    UInt32x4(uint32_t c0, uint32_t c1, uint32_t c2, uint32_t c3) {
    uint32_t v[4] = { c0, c1, c2, c3 };
        this->data = vld1q_u32(v);
    }

    UInt32x4(const int32_t data[4]) {
        this->data = (uint32x4_t)vld1q_s32(&data[0]);
    }

    UInt32x4(const uint32_t data[4]) {
         this->data = (uint32x4_t)vld1q_u32(&data[0]);
     }

    UInt32x4(const FixedVector<uint32_t,4> input)  {
        this->data = vld1q_u32(&input.element[0]);
    }


    /* Static fabrics */
    /** Load unaligned. Not safe to use until you exactly know what you are doing */

    static UInt32x4 load(const int32_t data[4])
    {
        return UInt32x4((uint32_t *)data);
    }

    static UInt32x4 load(const uint32_t data[4])
    {
       return UInt32x4(data);
    }

    /** Load aligned. Not safe to use untill you exactly know what you are doing */
    static UInt32x4 loadAligned(const uint32_t data[4])
    {
        return UInt32x4(data);
    }

    /* Savers */
    void save(uint32_t data[4]) const
    {
        vst1q_u32(&data[0], this->data);
    }

    void save(int32_t data[4]) const
    {
       vst1q_s32(&data[0], (int32x4_t)this->data);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(int32_t data[4]) const
    {
        vst1q_s32(&data[0], ( int32x4_t)this->data);
    }

    void saveAligned(uint32_t data[4]) const
    {
        vst1q_u32(&data[0], (uint32x4_t)this->data);
    }


    /** Stream aligned. Not safe to use untill you exactly know what you are doing */
    void streamAligned(int32_t data[4])
    {
        vst1q_s32(&data[0], (int32x4_t)this->data);
    }

    void streamAligned(uint32_t data[4])
    {
       vst1q_u32(&data[0], (uint32x4_t)this->data);
    }


    /* converters */
    uint32_t getInt(uint32_t idx)
    {
        CORE_ASSERT_TRUE(idx < 4, "Wrong idx in getInt()");
        return (uint32_t)vgetq_lane_u32(this->data, idx);
    }

//    inline uint16_t maskToInt2bit() const
//    {
//        return _mm_movemask_epi8(this->data);
//    }

    /* Logical operations */
    friend UInt32x4 operator &(const UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator |(const UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator ^(const UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 andNot    (const UInt32x4 &left, const UInt32x4 &right);

    friend UInt32x4 operator &=(UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator |=(UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator ^=(UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 andNotThis (UInt32x4 &left, const UInt32x4 &right);

    /* Arithmetics operations */
    friend UInt32x4 operator +(const UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator -(const UInt32x4 &left, const UInt32x4 &right);

    friend UInt32x4 operator +=(UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator -=(UInt32x4 &left, const UInt32x4 &right);

#if 0
    UInt32x4 operator -( ) {
        return (Int32x4((int32_t)0) - *this);
    }
#endif

    /* Immediate shift operations */
    friend UInt32x4 operator <<    (const UInt32x4 &left, uint32_t count);
    friend UInt32x4 operator >>    (const UInt32x4 &left, uint32_t count);
    friend UInt32x4 shiftLogical   (const UInt32x4 &left, uint32_t count);

    friend UInt32x4 operator <<=   (UInt32x4 &left, uint32_t count);
    friend UInt32x4 operator >>=   (UInt32x4 &left, uint32_t count);

    /* Shift operations */

    friend UInt32x4 operator <<    (const UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator >>    (const UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 shiftLogical   (const UInt32x4 &left, const UInt32x4 &right);

    friend UInt32x4 operator <<=   (UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator >>=   (UInt32x4 &left, const UInt32x4 &right);

    /* The division by constant*/
    friend UInt32x4 operator /     (const UInt32x4 &left, float divisor);
    friend UInt32x4 operator /     (const UInt32x4 &left, int divisor);
    friend UInt32x4 operator /=    (UInt32x4 &left, float divisor);
    friend UInt32x4 operator /=    (UInt32x4 &left, int divisor);

    /* Comparison */
    friend UInt32x4 operator <    (const UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator >    (const UInt32x4 &left, const UInt32x4 &right);
    friend UInt32x4 operator ==   (const UInt32x4 &left, const UInt32x4 &right);

    /* Shuffle */
//    template<int imm>
//    inline void shuffle()
//    {
//        this->data = _mm_shuffle_epi32(this->data, imm);
//    }

//    template<int imm>
//    inline Int32x4 shuffled() const
//    {
//        return Int32x4(_mm_shuffle_epi32(this->data, imm));
//    }

    inline void reverse()
    {
        *this = reversed();
    }

    inline UInt32x4 reversed() const
    {
        uint32x4_t tmp = vrev64q_u32(this->data);
        return vcombine_u32(vget_high_u32(tmp), vget_low_u32(tmp));
    }

    /* Print to stream helper */
    friend std::ostream & operator <<(std::ostream &out, const UInt32x4 &vector);


};

FORCE_INLINE UInt32x4 operator &(const UInt32x4 &left, const UInt32x4 &right) {
    return vandq_u32(left.data, right.data);
}

FORCE_INLINE UInt32x4 operator |(const UInt32x4 &left, const UInt32x4 &right) {
    return vorrq_u32(left.data, right.data);
}

FORCE_INLINE UInt32x4 operator ^(const UInt32x4 &left, const UInt32x4 &right) {
    return veorq_u32(left.data, right.data);
}

FORCE_INLINE UInt32x4 andNot    (const UInt32x4 &left, const UInt32x4 &right) {
    return vbicq_u32(left.data, right.data);
}

FORCE_INLINE UInt32x4 operator &=(UInt32x4 &left, const UInt32x4 &right) {
    left.data = vandq_u32(left.data, right.data);
    return left;
}

FORCE_INLINE UInt32x4 operator |=(UInt32x4 &left, const UInt32x4 &right) {
    left.data = vorrq_u32(left.data, right.data);
    return left;
}

FORCE_INLINE UInt32x4 operator ^=(UInt32x4 &left, const UInt32x4 &right) {
    left.data = veorq_u32(left.data, right.data);
    return left;
}

FORCE_INLINE UInt32x4 andNotThis (UInt32x4 &left, const UInt32x4 &right) {
    left.data = vbicq_u32(left.data, right.data);
    return left;
}

FORCE_INLINE UInt32x4 operator +(const UInt32x4 &left, const UInt32x4 &right) {
    return vaddq_u32(left.data, right.data);
}

FORCE_INLINE UInt32x4 operator -(const UInt32x4 &left, const UInt32x4 &right) {
    return vsubq_u32(left.data, right.data);
}

FORCE_INLINE UInt32x4 operator +=(UInt32x4 &left, const UInt32x4 &right) {
    left.data = vaddq_u32(left.data, right.data);
    return left;
}

FORCE_INLINE UInt32x4 operator -=(UInt32x4 &left, const UInt32x4 &right) {
    left.data = vsubq_u32(left.data, right.data);
    return left;
}

FORCE_INLINE UInt32x4 operator <<    (const UInt32x4 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        return vshlq_n_u32(left.data, count);
    else
        return vshlq_u32(left.data, vdupq_n_s32((int32_t)count));
}

FORCE_INLINE UInt32x4 shiftLogical (const UInt32x4 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        return vshrq_n_u32((uint32x4_t)left.data, (int32_t)count);
    else
        return vshlq_u32((uint32x4_t)left.data, vdupq_n_s32(-(int32_t)count));
}

FORCE_INLINE UInt32x4 operator >>(const UInt32x4 &left, uint32_t count) {
    return vshlq_u32(left.data, vdupq_n_s32(-(int32_t)count));
}

FORCE_INLINE UInt32x4 operator <<=(UInt32x4 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        left.data = vshlq_n_u32(left.data, count);
    else
        left.data = vshlq_u32(left.data, vdupq_n_s32((int32_t)count));

    return left;
}

FORCE_INLINE UInt32x4 operator >>=(UInt32x4 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        left.data = (uint32x4_t)vshrq_n_u32((uint32x4_t)left.data, (int32_t)count);
    else
        left.data = (uint32x4_t)vshlq_u32((uint32x4_t)left.data, vdupq_n_s32(-(int32_t)count));

    return left;
}

FORCE_INLINE UInt32x4 operator <<    (const UInt32x4 &left, const UInt32x4 &right) {
    int count = vgetq_lane_u32(right.data, 0);
    return vshlq_u32(left.data, vdupq_n_s32(count));
}

FORCE_INLINE UInt32x4 shiftLogcial (const UInt32x4 &left, const UInt32x4 &right) {
    int count = vgetq_lane_u32(right.data, 0);
    return vshlq_u32((uint32x4_t)left.data, vdupq_n_s32(-count));
}

FORCE_INLINE UInt32x4 operator >>  (const UInt32x4 &left, const UInt32x4 &right) {
    int count = vgetq_lane_u32(right.data, 0);
    return vshlq_u32(left.data, vdupq_n_s32(-count));
}

FORCE_INLINE UInt32x4 operator <<=(UInt32x4 &left, const UInt32x4 &right) {
    int count = vgetq_lane_u32(right.data, 0);
    left.data = vshlq_u32(left.data, vdupq_n_s32(count));
    return left;
}

FORCE_INLINE UInt32x4 operator >>=(UInt32x4 &left, const UInt32x4 &right) {
    int count = vgetq_lane_u32(right.data, 0);
    left.data = (uint32x4_t)vshlq_u32((uint32x4_t)left.data, vdupq_n_s32(-count));
    return left;
}

FORCE_INLINE UInt32x4 operator <    (const UInt32x4 &left, const UInt32x4 &right) {
    return vcltq_u32(left.data, right.data);
}

FORCE_INLINE UInt32x4 operator >    (const UInt32x4 &left, const UInt32x4 &right) {
    return vcgtq_u32(left.data, right.data);
}

FORCE_INLINE UInt32x4 operator ==   (const UInt32x4 &left, const UInt32x4 &right) {
    return vceqq_u32(left.data, right.data);
}
/*
FORCE_INLINE UInt32x4 unpackLower2  (const UInt32x4 &left, const UInt32x4 &right) {
    return (uint64x2_t)vzipq_u32(left.data, right.data).val[0];
}

FORCE_INLINE UInt32x4 unpackHigher2 (const UInt32x4 &left, const UInt32x4 &right) {
    return (uint64x2_t)vzipq_u32(left.data, right.data).val[1];
}
*/

} //namespace corecvs
#endif  //INT32X4_H_

