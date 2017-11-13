#ifndef INT32X4_H_
#define INT32X4_H_
/**
 * \file int32x4.h
 * \brief a header for Int32x4.c
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include <arm_neon.h>
#include <stdint.h>

#include "core/utils/global.h"
#include "core/math/vector/fixedVector.h"
#include "core/math/sse/int64x2.h"
namespace corecvs {

class ALIGN_DATA(16) Int32x4
{
public:
    /**
     *  Handle to SSE 128 value.
     *
     *  NB Union here dramatically decrease speed. So don't use one
     **/

    int32x4_t data;

    /* Constructors */
    Int32x4(){};
    /**
     *  Copy constructor
     **/
    Int32x4(const Int32x4 &other) {
        this->data = other.data;
    }

    /**
     *  Create SSE integer vector from integer constant
     **/
    Int32x4(const int32x4_t &_data) {
        this->data = _data;
    }

    Int32x4(const uint32x4_t &_data) {
        this->data = (int32x4_t)_data;
    }

    Int32x4(int32_t constant) {
        this->data = vdupq_n_s32(constant);
    }

    Int32x4(int32_t c0, int32_t c1, int32_t c2, int32_t c3) {
    int32_t v[4] = { c0, c1, c2, c3 };
        this->data = vld1q_s32(v);
    }

    Int32x4(const int32_t data[4]) {
        this->data = vld1q_s32(&data[0]);
    }

    Int32x4(const uint32_t data[4]) {
          this->data = (int32x4_t)vld1q_u32(&data[0]);
      }

    Int32x4(const FixedVector<int32_t,4> input)  {
        this->data = vld1q_s32(&input.element[0]);
    }


    /* Static fabrics */
    /** Load unaligned. Not safe to use until you exactly know what you are doing */

    static Int32x4 load(const int32_t data[4])
    {
        return Int32x4(data);
    }

    static Int32x4 load(const uint32_t data[4])
    {
       return Int32x4(data);
    }

    /** Load aligned. Not safe to use untill you exactly know what you are doing */
    static Int32x4 loadAligned(const int32_t data[4])
    {
        return Int32x4(data);
    }

    /* Savers */
    void save(int32_t data[4]) const
    {
        vst1q_s32(&data[0], this->data);
    }

    void save(uint32_t data[4]) const
    {
       vst1q_u32(&data[0], (uint32x4_t)this->data);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(int32_t data[4]) const
    {
        vst1q_s32(&data[0], this->data);
    }

    void saveAligned(uint32_t data[4]) const
    {
        vst1q_u32(&data[0], (uint32x4_t)this->data);
    }


    /** Stream aligned. Not safe to use untill you exactly know what you are doing */
    void streamAligned(int32_t data[4])
    {
        vst1q_s32(&data[0], this->data);
    }

    void streamAligned(uint32_t data[4])
    {
       vst1q_u32(&data[0], (uint32x4_t)this->data);
    }


    /* converters */
    uint32_t getInt(uint32_t idx)
    {
        ASSERT_TRUE(idx < 4, "Wrong idx in getInt()");
        return (uint32_t)vgetq_lane_s32(this->data, idx);
    }

//    inline uint16_t maskToInt2bit() const
//    {
//        return _mm_movemask_epi8(this->data);
//    }

    /* Logical operations */
    friend Int32x4 operator &(const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator |(const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator ^(const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 andNot    (const Int32x4 &left, const Int32x4 &right);

    friend Int32x4 operator &=(Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator |=(Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator ^=(Int32x4 &left, const Int32x4 &right);
    friend Int32x4 andNotThis (Int32x4 &left, const Int32x4 &right);

    /* Arithmetics operations */
    friend Int32x4 operator +(const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator -(const Int32x4 &left, const Int32x4 &right);

    friend Int32x4 operator +=(Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator -=(Int32x4 &left, const Int32x4 &right);

    Int32x4 operator -( ) {
        return (Int32x4((int32_t)0) - *this);
    };

    /* Immediate shift operations */
    friend Int32x4 operator <<    (const Int32x4 &left, uint32_t count);
    friend Int32x4 operator >>    (const Int32x4 &left, uint32_t count);
    friend Int32x4 shiftArithmetic(const Int32x4 &left, uint32_t count);

    friend Int32x4 operator <<=   (Int32x4 &left, uint32_t count);
    friend Int32x4 operator >>=   (Int32x4 &left, uint32_t count);

    /* Shift operations */

    friend Int32x4 operator <<    (const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator >>    (const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 shiftArithmetic(const Int32x4 &left, const Int32x4 &right);

    friend Int32x4 operator <<=   (Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator >>=   (Int32x4 &left, const Int32x4 &right);

    /* The division by constant*/
    friend Int32x4 operator /     (const Int32x4 &left, float divisor);
    friend Int32x4 operator /     (const Int32x4 &left, int divisor);
    friend Int32x4 operator /=    (Int32x4 &left, float divisor);
    friend Int32x4 operator /=    (Int32x4 &left, int divisor);

    /* Comparison */
    friend Int32x4 operator <    (const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator >    (const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator ==   (const Int32x4 &left, const Int32x4 &right);

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

    inline Int32x4 reversed() const
    {
    int32x4_t tmp = vrev64q_s32(this->data);
        return vcombine_s32(vget_high_s32(tmp), vget_low_s32(tmp));
    }

    /* Print to stream helper */
    friend ostream & operator <<(ostream &out, const Int32x4 &vector);


};

FORCE_INLINE inline Int32x4 operator &(const Int32x4 &left, const Int32x4 &right) {
    return vandq_s32(left.data, right.data);
}

FORCE_INLINE inline Int32x4 operator |(const Int32x4 &left, const Int32x4 &right) {
    return vorrq_s32(left.data, right.data);
}

FORCE_INLINE inline Int32x4 operator ^(const Int32x4 &left, const Int32x4 &right) {
    return veorq_s32(left.data, right.data);
}

FORCE_INLINE inline Int32x4 andNot    (const Int32x4 &left, const Int32x4 &right) {
    return vbicq_s32(left.data, right.data);
}

FORCE_INLINE inline Int32x4 operator &=(Int32x4 &left, const Int32x4 &right) {
    left.data = vandq_s32(left.data, right.data);
    return left;
}

FORCE_INLINE inline Int32x4 operator |=(Int32x4 &left, const Int32x4 &right) {
    left.data = vorrq_s32(left.data, right.data);
    return left;
}

FORCE_INLINE inline Int32x4 operator ^=(Int32x4 &left, const Int32x4 &right) {
    left.data = veorq_s32(left.data, right.data);
    return left;
}

FORCE_INLINE inline Int32x4 andNotThis (Int32x4 &left, const Int32x4 &right) {
    left.data = vbicq_s32(left.data, right.data);
    return left;
}

FORCE_INLINE inline Int32x4 operator +(const Int32x4 &left, const Int32x4 &right) {
    return vaddq_s32(left.data, right.data);
}

FORCE_INLINE inline Int32x4 operator -(const Int32x4 &left, const Int32x4 &right) {
    return vsubq_s32(left.data, right.data);
}

FORCE_INLINE inline Int32x4 operator +=(Int32x4 &left, const Int32x4 &right) {
    left.data = vaddq_s32(left.data, right.data);
    return left;
}

FORCE_INLINE inline Int32x4 operator -=(Int32x4 &left, const Int32x4 &right) {
    left.data = vsubq_s32(left.data, right.data);
    return left;
}

FORCE_INLINE inline Int32x4 operator <<    (const Int32x4 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        return vshlq_n_s32(left.data, count);
    else
        return vshlq_s32(left.data, vdupq_n_s32((int32_t)count));
}

FORCE_INLINE inline Int32x4 operator >>    (const Int32x4 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        return vshrq_n_u32((uint32x4_t)left.data, (int32_t)count);
    else
        return vshlq_u32((uint32x4_t)left.data, vdupq_n_s32(-(int32_t)count));
}

FORCE_INLINE inline Int32x4 shiftArithmetic(const Int32x4 &left, uint32_t count) {
    return vshlq_s32(left.data, vdupq_n_s32(-(int32_t)count));
}

FORCE_INLINE inline Int32x4 operator <<=(Int32x4 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        left.data = vshlq_n_s32(left.data, count);
    else
        left.data = vshlq_s32(left.data, vdupq_n_s32((int32_t)count));

    return left;
}

FORCE_INLINE inline Int32x4 operator >>=(Int32x4 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        left.data = (int32x4_t)vshrq_n_u32((uint32x4_t)left.data, (int32_t)count);
    else
        left.data = (int32x4_t)vshlq_u32((uint32x4_t)left.data, vdupq_n_s32(-(int32_t)count));

    return left;
}

FORCE_INLINE inline Int32x4 operator <<    (const Int32x4 &left, const Int32x4 &right) {
    int count = vgetq_lane_s32(right.data, 0);
    return vshlq_s32(left.data, vdupq_n_s32(count));
}

FORCE_INLINE inline Int32x4 operator >>    (const Int32x4 &left, const Int32x4 &right) {
    int count = vgetq_lane_s32(right.data, 0);
    return vshlq_u32((uint32x4_t)left.data, vdupq_n_s32(-count));
}

FORCE_INLINE inline Int32x4 shiftArithmetic(const Int32x4 &left, const Int32x4 &right) {
    int count = vgetq_lane_s32(right.data, 0);
    return vshlq_s32(left.data, vdupq_n_s32(-count));
}

FORCE_INLINE inline Int32x4 operator <<=(Int32x4 &left, const Int32x4 &right) {
    int count = vgetq_lane_s32(right.data, 0);
    left.data = vshlq_s32(left.data, vdupq_n_s32(count));
    return left;
}

FORCE_INLINE inline Int32x4 operator >>=(Int32x4 &left, const Int32x4 &right) {
    int count = vgetq_lane_s32(right.data, 0);
    left.data = (int32x4_t)vshlq_u32((uint32x4_t)left.data, vdupq_n_s32(-count));
    return left;
}

FORCE_INLINE inline Int32x4 operator <    (const Int32x4 &left, const Int32x4 &right) {
    return vcltq_s32(left.data, right.data);
}

FORCE_INLINE inline Int32x4 operator >    (const Int32x4 &left, const Int32x4 &right) {
    return vcgtq_s32(left.data, right.data);
}

FORCE_INLINE inline Int32x4 operator ==   (const Int32x4 &left, const Int32x4 &right) {
    return vceqq_s32(left.data, right.data);
}

FORCE_INLINE inline Int64x2 unpackLower2  (const Int32x4 &left, const Int32x4 &right) {
    return (int64x2_t)vzipq_s32(left.data, right.data).val[0];
}

FORCE_INLINE inline Int64x2 unpackHigher2 (const Int32x4 &left, const Int32x4 &right) {
    return (int64x2_t)vzipq_s32(left.data, right.data).val[1];
}


} //namespace corecvs
#endif  //INT32X4_H_

