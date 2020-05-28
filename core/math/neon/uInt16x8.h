#ifndef UINT16X8_H_
#define UINT16X8_H_
/**
 * \file uint16x8.h
 * \brief
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include <arm_neon.h>
#include <stdint.h>

#include "utils/global.h"
#include "math/vector/fixedVector.h"
namespace corecvs {


class ALIGN_DATA(16) UInt16x8
{
public:
    static const int SIZE = 8;


    uint16x8_t data;
    /* Constructors */

    UInt16x8(){}

    /**
     *  Copy constructor
     **/
    UInt16x8(const Int16x8 &other) {
        this->data = (uint16x8_t)other.data;
    }

    /**
     *  Create SSE integer vector from integer constant
     **/
    UInt16x8(const int16x8_t &_data) {
        this->data = (uint16x8_t)_data;
    }

    UInt16x8(const uint16x8_t &_data) {
        this->data = _data;
    }

    explicit UInt16x8(int16_t constant) {
        this->data = vdupq_n_u16(constant);
    }

    explicit UInt16x8(uint16_t constant) {
        this->data = (uint16x8_t)vdupq_n_u16(constant);
    }

    explicit UInt16x8(uint16_t c0, uint16_t c1, uint16_t c2, uint16_t c3,
            uint16_t c4, uint16_t c5, uint16_t c6, uint16_t c7) {
    uint16_t v[8] = { c0, c1, c2, c3, c4, c5, c6, c7 };
        this->data = vld1q_u16(v);
    }

    explicit UInt16x8(const uint16_t data[8]) {
        this->data = vld1q_u16(&data[0]);
    }

    explicit UInt16x8(const int16_t data[8]) {
        this->data = (uint16x8_t)vld1q_s16(&data[0]);
    }

    explicit inline UInt16x8(const FixedVector<uint16_t,8> input)  {
        this->data = vld1q_u16(&input.element[0]);
    }

    explicit inline UInt16x8(const Int32x4 &value)
    {
        this->data = (uint16x8_t)value.data;
    }

    explicit operator Int16x8() {
        return Int16x8(data);
    }


    /*
    explicit inline Int16x8(const Int32x8 &value)
    {
        this->data = vcombine_s16(vqmovn_s32(value.element[0].data), vqmovn_s32(value.element[1].data));
    }
    */

    /* Static fabrics */

    static UInt16x8 pack(const UInt32x4 &first, const UInt32x4 &second)
    {
        return UInt16x8(vcombine_u16(vqmovn_u32(first.data), vqmovn_u32(second.data)));
    }

    /** Load unaligned. Not safe to use until you exactly know what you are doing */
    static UInt16x8 load(const uint16_t *data)
    {
        return UInt16x8(data);
    }

    /** Load aligned. Not safe to use untill you exactly know what you are doing */
    static UInt16x8 loadAligned(const uint16_t data[8])
    {
        return UInt16x8(data);
    }

    /* Savers */
#ifdef PROFILE_ACCESS_ALIGNMENT
static uint64_t   alignedUWrites;
static uint64_t   alignedAWrites;
static uint64_t unalignedUWrites;
static uint64_t   streamedWrites;

    static void resetAlignmnetCouters(void)
    {
        alignedUWrites = 0;
        alignedAWrites = 0;
        unalignedUWrites = 0;
        streamedWrites = 0;
    }


#endif

    void save(uint16_t data[8]) const
    {
        vst1q_u16(&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        if (((uint64_t)(void *)&data[0]) % 16)
            unalignedUWrites++;
        else
            alignedUWrites++;
#endif
    }

    void save(int16_t data[8]) const
    {
        save((uint16_t *)&data[0]);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(uint16_t data[8]) const
    {
        vst1q_u16(&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
         alignedAWrites++;
#endif
    }

    void saveAligned(int16_t data[8]) const
    {
        save((uint16_t *)&data[0]);
    }

    /** Stream aligned. Not safe to use until you exactly know what you are doing */
    void streamAligned(uint16_t data[8]) const
    {
        vst1q_u16(&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        streamedWrites++;
#endif
    }

    /* converters */
template<int idx>
    uint16_t getInt() const
    {
        return vgetq_lane_u16(this->data, idx);
    }

    inline uint16_t operator[] (uint32_t idx) const
    {
        switch (idx) {
            case 0: return getInt<0>();
            case 1: return getInt<1>();
            case 2: return getInt<2>();
            case 3: return getInt<3>();
            case 4: return getInt<4>();
            case 5: return getInt<5>();
            case 6: return getInt<6>();
            case 7: return getInt<7>();
        }
        return 0;
    }

//    inline uint16_t maskToInt() const
//    {
//        return _mm_movemask_epi8(this->data);
//    }
/*
    inline Int32x8 expand() const
    {
        return Int32x8(
                unpackLower4 (*this, Int16x8((uint16_t)0)),
                unpackHigher4(*this, Int16x8((uint16_t)0))
                );
    }
*/
    /* Logical operations */
    friend UInt16x8 operator &(const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator |(const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator ^(const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 andNot    (const UInt16x8 &left, const UInt16x8 &right);

    friend UInt16x8 operator &=(UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator |=(UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator ^=(UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 andNotThis (UInt16x8 &left, const UInt16x8 &right);

    /* Arithmetics operations */
    friend UInt16x8 operator +(const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator -(const UInt16x8 &left, const UInt16x8 &right);

    friend UInt16x8 operator +=(UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator -=(UInt16x8 &left, const UInt16x8 &right);

    /* Immediate shift operations */
    friend UInt16x8 operator <<    (const UInt16x8 &left, uint32_t count);
    friend UInt16x8 operator >>    (const UInt16x8 &left, uint32_t count);
    friend UInt16x8 shiftArithmetic(const UInt16x8 &left, uint32_t count);
    friend UInt16x8 shrAdd(const UInt16x8 &accumulator, const UInt16x8 &left, uint32_t count);

    friend UInt16x8 operator <<=   (UInt16x8 &left, uint32_t count);
    friend UInt16x8 operator >>=   (UInt16x8 &left, uint32_t count);

    /* Shift operations */

    friend UInt16x8 operator <<    (const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator >>    (const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 shiftArithmetic(const UInt16x8 &left, const UInt16x8 &right);

    friend UInt16x8 operator <<=   (UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator >>=   (UInt16x8 &left, const UInt16x8 &right);


    /* Comparison */
    friend UInt16x8 operator <    (const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator >    (const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator ==   (const UInt16x8 &left, const UInt16x8 &right);

    /* Multiplication beware - overrun is possible*/
    friend UInt16x8 operator *    (const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator *=   (const UInt16x8 &left, const UInt16x8 &right);

    friend UInt16x8 productLowerPart  (const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 productHigherPart (const UInt16x8 &left, const UInt16x8 &right);

//    friend Int32x8 productExtending (const UInt16x8 &left, const UInt16x8 &right);

    /* Slow but helpful */
    friend UInt16x8 operator *    (      int16_t right, UInt16x8 &left);
    friend UInt16x8 operator *    (const UInt16x8 &left, int16_t right);
    friend UInt16x8 operator *=   (      UInt16x8 &left, int16_t right);


    friend UInt32x4 unpackLower4  (const UInt16x8 &left, const UInt16x8 &right);
    friend UInt32x4 unpackHigher4 (const UInt16x8 &left, const UInt16x8 &right);

    /*Print to stream helper */

    friend ostream & operator <<(ostream &out, const UInt16x8 &vector);

    /* Shuffle */
//    template<int imm>
//    inline void shuffleLower()
//    {
//        this->data = _mm_shufflelo_epi16(this->data, imm);
//    }

//    template<int imm>
//    inline void shuffleHigher()
//    {
//        this->data = _mm_shufflehi_epi16(this->data, imm);
//    }

    inline void reverse()
    {
        uint16x8_t tmp = vrev64q_u16(this->data);
        this->data = vcombine_u16(vget_high_u16(tmp), vget_low_u16(tmp));
    }

};

FORCE_INLINE UInt16x8 operator &(const UInt16x8 &left, const UInt16x8 &right) {
    return vandq_u16(left.data, right.data);
}

FORCE_INLINE UInt16x8 operator |(const UInt16x8 &left, const UInt16x8 &right) {
    return vorrq_u16(left.data, right.data);
}

FORCE_INLINE UInt16x8 operator ^(const UInt16x8 &left, const UInt16x8 &right) {
    return veorq_u16(left.data, right.data);
}

FORCE_INLINE UInt16x8 andNot    (const UInt16x8 &left, const UInt16x8 &right) {
    return vbicq_u16(left.data, right.data);
}

FORCE_INLINE UInt16x8 operator &=(UInt16x8 &left, const UInt16x8 &right) {
    left.data = vandq_u16(left.data, right.data);
    return left;
}

FORCE_INLINE UInt16x8 operator |=(UInt16x8 &left, const UInt16x8 &right) {
    left.data = vorrq_u16(left.data, right.data);
    return left;
}

FORCE_INLINE UInt16x8 operator ^=(UInt16x8 &left, const UInt16x8 &right) {
    left.data = veorq_u16(left.data, right.data);
    return left;
}

FORCE_INLINE UInt16x8 andNotThis (UInt16x8 &left, const UInt16x8 &right) {
    left.data = vbicq_u16(left.data, right.data);
    return left;
}

FORCE_INLINE UInt16x8 operator +(const UInt16x8 &left, const UInt16x8 &right) {
    return UInt16x8(vaddq_u16(left.data, right.data));
}

FORCE_INLINE UInt16x8 operator -(const UInt16x8 &left, const UInt16x8 &right) {
    return UInt16x8(vsubq_u16(left.data, right.data));
}

FORCE_INLINE UInt16x8 operator +=(UInt16x8 &left, const UInt16x8 &right) {
    left.data = vaddq_u16(left.data, right.data);
    return left;
}

FORCE_INLINE UInt16x8 operator -=(UInt16x8 &left, const UInt16x8 &right) {
    left.data = vsubq_u16(left.data, right.data);
    return left;
}

FORCE_INLINE UInt16x8 operator <<    (const UInt16x8 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        return vshlq_n_u16(left.data, (int)count);
    else
        return vshlq_u16(left.data, vdupq_n_s16((int)count));
}

FORCE_INLINE UInt16x8 operator >>    (const UInt16x8 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        return vshrq_n_u16((uint16x8_t)left.data, (int)count);
    else
        return vshlq_u16((uint16x8_t)left.data, vdupq_n_s16(-(int)count));
}

FORCE_INLINE UInt16x8 shiftArithmetic(const UInt16x8 &left, uint32_t count) {
    return vshlq_u16(left.data, vdupq_n_s16(-(int)count));
}

FORCE_INLINE UInt16x8 shrAdd(const UInt16x8 &accumulator, const UInt16x8 &left, uint32_t count) {
    return vsraq_n_u16(accumulator.data, left.data, count);
}

FORCE_INLINE UInt16x8 operator <<=(UInt16x8 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        left.data = vshlq_n_u16(left.data, (int)count);
    else
        left.data = vshlq_u16(left.data, vdupq_n_s16((int)count));

    return left;
}

FORCE_INLINE UInt16x8 operator >>=(UInt16x8 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        left.data = vshrq_n_u16(left.data, (int)count);
    else
        left.data = vshlq_u16(left.data, vdupq_n_s16(-(int)count));
    return left;
}

FORCE_INLINE UInt16x8 operator <<    (const UInt16x8 &left, const UInt16x8 &right) {
    int count = vgetq_lane_u16(right.data, 0);
    return vshlq_u16(left.data, vdupq_n_s16(count));
}

FORCE_INLINE UInt16x8 operator >>    (const UInt16x8 &left, const UInt16x8 &right) {
    int count = vgetq_lane_u16(right.data, 0);
    return vshlq_u16((uint16x8_t)left.data, vdupq_n_s16(-count));
}

FORCE_INLINE UInt16x8 shiftArithmetic(const UInt16x8 &left, const UInt16x8 &right) {
    int count = vgetq_lane_u16(right.data, 0);
    return vshlq_u16(left.data, vdupq_n_s16(-count));
}

FORCE_INLINE UInt16x8 operator <<=(UInt16x8 &left, const UInt16x8 &right) {
    int count = vgetq_lane_u16(right.data, 0);
    left.data = vshlq_u16(left.data, vdupq_n_s16(count));
    return left;
}

FORCE_INLINE UInt16x8 operator >>=(UInt16x8 &left, const UInt16x8 &right) {
    int count = vgetq_lane_u16(right.data, 0);
    left.data = vshlq_u16((uint16x8_t)left.data, vdupq_n_s16(-count));
    return left;
}

FORCE_INLINE UInt16x8 operator <    (const UInt16x8 &left, const UInt16x8 &right) {
    return UInt16x8(vcltq_u16(left.data, right.data));
}

FORCE_INLINE UInt16x8 operator >    (const UInt16x8 &left, const UInt16x8 &right) {
    return UInt16x8(vcgtq_u16(left.data, right.data));
}

FORCE_INLINE UInt16x8 operator ==   (const UInt16x8 &left, const UInt16x8 &right) {
    return UInt16x8(vceqq_u16(left.data, right.data));
}

FORCE_INLINE UInt16x8 productLowerPart  (const UInt16x8 &left, const UInt16x8 &right) {
    return vmulq_u16(left.data, right.data);
}

/*
FORCE_INLINE UInt16x8 productHigherPart (const UInt16x8 &left, const UInt16x8 &right) {
    return vqdmulhq_u16(left.data, right.data);
}
*/

FORCE_INLINE UInt16x8 operator *    (const UInt16x8 &left, const UInt16x8 &right) {
    return productLowerPart(left, right);
}

FORCE_INLINE UInt16x8 operator *=   (UInt16x8 &left, const UInt16x8 &right) {
    left = productLowerPart(left, right);
    return left;
}


FORCE_INLINE UInt32x4 unpackLower4  (const UInt16x8 &left, const UInt16x8 &right) {
    return (uint32x4_t)vzipq_u16(left.data, right.data).val[0];
}

FORCE_INLINE UInt32x4 unpackHigher4 (const UInt16x8 &left, const UInt16x8 &right) {
    return (uint32x4_t)vzipq_u16(left.data, right.data).val[1];
}


/*
FORCE_INLINE Int32x8 productExtending (const Int16x8 &left, const Int16x8 &right)
{
    Int16x8  lowParts;
    Int16x8 highParts;
    lowParts  = productLowerPart (left, right);
    highParts = productHigherPart(left, right);
    return Int32x8(unpackLower4(lowParts,highParts), unpackHigher4 (lowParts,highParts));
}
*/
FORCE_INLINE UInt16x8 operator *  (      uint16_t left, UInt16x8 &right) {
    return right * UInt16x8(left);
}

FORCE_INLINE UInt16x8 operator *  (const UInt16x8 &left, uint16_t right) {
    return left * UInt16x8(right);
}

FORCE_INLINE UInt16x8 operator *= (UInt16x8 &left, uint16_t right) {
    left = left * right;
    return left;
}

} //namespace corecvs
#endif //#ifndef INT16X8_H_

