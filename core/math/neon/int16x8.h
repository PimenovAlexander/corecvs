#ifndef INT16X8_H_
#define INT16X8_H_
/**
 * \file int16x8.h
 * \brief
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include <arm_neon.h>
#include <stdint.h>

#include "core/utils/global.h"
#include "core/math/vector/fixedVector.h"
namespace corecvs {


class ALIGN_DATA(16) Int16x8
{
public:
    static const int SIZE = 8;


    int16x8_t data;
    /* Constructors */

    Int16x8(){}

    /**
     *  Copy constructor
     **/
    Int16x8(const Int16x8 &other) {
        this->data = other.data;
    }

    /**
     *  Create SSE integer vector from integer constant
     **/
    Int16x8(const int16x8_t &_data) {
        this->data = _data;
    }

    Int16x8(const uint16x8_t &_data) {
        this->data = (int16x8_t)_data;
    }

    explicit Int16x8(int16_t constant) {
        this->data = vdupq_n_s16(constant);
    }

    explicit Int16x8(uint16_t constant) {
        this->data = (int16x8_t)vdupq_n_u16(constant);
    }

    explicit Int16x8(int16_t c0, int16_t c1, int16_t c2, int16_t c3,
            int16_t c4, int16_t c5, int16_t c6, int16_t c7) {
    int16_t v[8] = { c0, c1, c2, c3, c4, c5, c6, c7 };
        this->data = vld1q_s16(v);
    }

    explicit Int16x8(const int16_t data[8]) {
        this->data = vld1q_s16(&data[0]);
    }

    explicit Int16x8(const uint16_t data[8]) {
        this->data = (int16x8_t)vld1q_u16(&data[0]);
    }

    explicit inline Int16x8(const FixedVector<int16_t,8> input)  {
        this->data = vld1q_s16(&input.element[0]);
    }

    explicit inline Int16x8(const Int32x4 &value)
    {
        this->data = (int16x8_t)value.data;
    }


    explicit inline Int16x8(const Int32x8v &value)
    {
        this->data = vcombine_s16(vqmovn_s32(value.element[0].data), vqmovn_s32(value.element[1].data));
    }


    /* Static fabrics */

    static Int16x8 pack(const Int32x4 &first, const Int32x4 &second)
    {
        return Int16x8(vcombine_s16(vqmovn_s32(first.data), vqmovn_s32(second.data)));
    }

    /** Load unaligned. Not safe to use until you exactly know what you are doing */
    static Int16x8 load(const int16_t *data)
    {
        return Int16x8(data);
    }

    /** Load aligned. Not safe to use untill you exactly know what you are doing */
    static Int16x8 loadAligned(const int16_t data[8])
    {
        return Int16x8(data);
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

    void save(int16_t data[8]) const
    {
        vst1q_s16(&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        if (((uint64_t)(void *)&data[0]) % 16)
            unalignedUWrites++;
        else
            alignedUWrites++;
#endif
    }

    void save(uint16_t data[8]) const
    {
        save((int16_t *)&data[0]);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(int16_t data[8]) const
    {
        vst1q_s16(&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
         alignedAWrites++;
#endif
    }

    void saveAligned(uint16_t data[8]) const
    {
        save((int16_t *)&data[0]);
    }

    /** Stream aligned. Not safe to use until you exactly know what you are doing */
    void streamAligned(int16_t data[8]) const
    {
        vst1q_s16(&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        streamedWrites++;
#endif
    }

    /* converters */
template<int idx>
    uint16_t getInt() const
    {
        return vgetq_lane_s16(this->data, idx);
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

    inline uint16_t maskToInt() const
    {
        const uint8_t __attribute__ ((aligned (16))) powers_data[16]=
            { 1, 2, 4, 8, 16, 32, 64, 128, 1, 2, 4, 8, 16, 32, 64, 128 };

        // Set the powers of 2 (do it once for all, if applicable)
        uint8x16_t powers= vld1q_u8(powers_data);

        // Compute the mask from the input
        uint64x2_t mask= vpaddlq_u32(vpaddlq_u16(vpaddlq_u8(vandq_u8((uint8x16_t)data, powers))));

        // Get the resulting bytes
        uint16_t out;
        vst1q_lane_u8((uint8_t*)&out + 0, (uint8x16_t)mask, 0);
        vst1q_lane_u8((uint8_t*)&out + 1, (uint8x16_t)mask, 8);

        return out;
    }



    inline Int32x8v expand() const
    {
        return Int32x8v(
                unpackLower (*this, Int16x8((uint16_t)0)),
                unpackHigher(*this, Int16x8((uint16_t)0))
                );
    }

    /* Logical operations */
    friend Int16x8 operator &(const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator |(const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator ^(const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 andNot    (const Int16x8 &left, const Int16x8 &right);

    friend Int16x8 operator &=(Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator |=(Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator ^=(Int16x8 &left, const Int16x8 &right);
    friend Int16x8 andNotThis (Int16x8 &left, const Int16x8 &right);

    /* Arithmetics operations */
    friend Int16x8 operator +(const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator -(const Int16x8 &left, const Int16x8 &right);

    friend Int16x8 operator +=(Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator -=(Int16x8 &left, const Int16x8 &right);

    Int16x8 operator -( ) {
        return (Int16x8((int16_t)0) - *this);
    }

    /* Immediate shift operations */
    friend Int16x8  operator <<    (const Int16x8 &left, uint32_t count);
    friend Int16x8  operator >>    (const Int16x8 &left, uint32_t count);
    friend Int16x8  shiftArithmetic(const Int16x8 &left, uint32_t count);
    friend Int16x8  shiftLogical   (const Int16x8 &left, uint32_t count);
           Int16x8& shiftLogical   (uint32_t count);


    friend Int16x8 shrAdd(const Int16x8 &accumulator, const Int16x8 &left, uint32_t count);

    friend Int16x8 operator <<=   (Int16x8 &left, uint32_t count);
    friend Int16x8 operator >>=   (Int16x8 &left, uint32_t count);

    /* Shift operations */

    friend Int16x8 operator <<    (const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator >>    (const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 shiftArithmetic(const Int16x8 &left, const Int16x8 &right);

    friend Int16x8 operator <<=   (Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator >>=   (Int16x8 &left, const Int16x8 &right);


    /* Comparison */
    friend Int16x8 operator <    (const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator >    (const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator ==   (const Int16x8 &left, const Int16x8 &right);

    /* Multiplication beware - overrun is possible*/
    friend Int16x8 operator *    (const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator *=   (const Int16x8 &left, const Int16x8 &right);

    friend Int16x8 productLowerPart  (const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 productHigherPart (const Int16x8 &left, const Int16x8 &right);

//    friend Int32x8 productExtending (const Int16x8 &left, const Int16x8 &right);

    /* Slow but helpful */
    friend Int16x8 operator *    (      int16_t right, Int16x8 &left);
    friend Int16x8 operator *    (const Int16x8 &left, int16_t right);
    friend Int16x8 operator *=   (      Int16x8 &left, int16_t right);


    static inline Int32x4 unpackLower  (const Int16x8 &left, const Int16x8 &right) {
        return (int32x4_t)vzipq_s16(left.data, right.data).val[0];
    }

    static inline Int32x4 unpackHigher (const Int16x8 &left, const Int16x8 &right) {
        return (int32x4_t)vzipq_s16(left.data, right.data).val[1];
    }

    /*Print to stream helper */

    friend ostream & operator <<(ostream &out, const Int16x8 &vector);

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
        int16x8_t tmp = vrev64q_s16(this->data);
        this->data = vcombine_s16(vget_high_s16(tmp), vget_low_s16(tmp));
    }

    void saveLower(uint16_t data[4]) const
    {
        vst1q_lane_u64((uint64_t *)data, (uint64x2_t)this->data, 0);
    }

};

FORCE_INLINE Int16x8 operator &(const Int16x8 &left, const Int16x8 &right) {
    return vandq_s16(left.data, right.data);
}

FORCE_INLINE Int16x8 operator |(const Int16x8 &left, const Int16x8 &right) {
    return vorrq_s16(left.data, right.data);
}

FORCE_INLINE Int16x8 operator ^(const Int16x8 &left, const Int16x8 &right) {
    return veorq_s16(left.data, right.data);
}

FORCE_INLINE Int16x8 andNot    (const Int16x8 &left, const Int16x8 &right) {
//    return vbicq_s16(left.data, right.data);
     return vbicq_s16(right.data, left.data);
}

FORCE_INLINE Int16x8 operator &=(Int16x8 &left, const Int16x8 &right) {
    left.data = vandq_s16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 operator |=(Int16x8 &left, const Int16x8 &right) {
    left.data = vorrq_s16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 operator ^=(Int16x8 &left, const Int16x8 &right) {
    left.data = veorq_s16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 andNotThis (Int16x8 &left, const Int16x8 &right) {
    left.data = vbicq_s16(right.data, left.data);
    return left;
}

FORCE_INLINE Int16x8 operator +(const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(vaddq_s16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator -(const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(vsubq_s16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator +=(Int16x8 &left, const Int16x8 &right) {
    left.data = vaddq_s16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 operator -=(Int16x8 &left, const Int16x8 &right) {
    left.data = vsubq_s16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 operator <<    (const Int16x8 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        return vshlq_n_s16(left.data, (int)count);
    else
        return vshlq_s16(left.data, vdupq_n_s16((int)count));
}

FORCE_INLINE Int16x8 operator >>    (const Int16x8 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        return vshrq_n_u16((uint16x8_t)left.data, (int)count);
    else
        return vshlq_u16((uint16x8_t)left.data, vdupq_n_s16(-(int)count));
}

FORCE_INLINE Int16x8 shiftLogical(const Int16x8 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        return vshrq_n_u16((uint16x8_t)left.data, (int)count);
    else
        return vshlq_u16((uint16x8_t)left.data, vdupq_n_s16(-(int)count));
}

FORCE_INLINE Int16x8& Int16x8::shiftLogical(uint32_t count) {
    if ( __builtin_constant_p(count) ) {
        data = (int16x8_t)vshrq_n_u16((uint16x8_t)data, (int)count);
    } else {
        data = (int16x8_t)vshlq_u16((uint16x8_t)data, vdupq_n_s16(-(int)count));
    }
    return *this;
}

FORCE_INLINE Int16x8 shiftArithmetic(const Int16x8 &left, uint32_t count) {
    return vshlq_s16(left.data, vdupq_n_s16(-(int)count));
}

FORCE_INLINE Int16x8 shrAdd(const Int16x8 &accumulator, const Int16x8 &left, uint32_t count) {
    return vsraq_n_s16(accumulator.data, left.data, count);
}

FORCE_INLINE Int16x8 operator <<=(Int16x8 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        left.data = vshlq_n_s16(left.data, (int)count);
    else
        left.data = vshlq_s16(left.data, vdupq_n_s16((int)count));

    return left;
}

FORCE_INLINE Int16x8 operator >>=(Int16x8 &left, uint32_t count) {
    if ( __builtin_constant_p(count) )
        left.data = vshrq_n_s16(left.data, (int)count);
    else
        left.data = vshlq_s16(left.data, vdupq_n_s16(-(int)count));
    return left;
}

FORCE_INLINE Int16x8 operator <<    (const Int16x8 &left, const Int16x8 &right) {
    int count = vgetq_lane_s16(right.data, 0);
    return vshlq_s16(left.data, vdupq_n_s16(count));
}

FORCE_INLINE Int16x8 operator >>    (const Int16x8 &left, const Int16x8 &right) {
    int count = vgetq_lane_s16(right.data, 0);
    return vshlq_u16((uint16x8_t)left.data, vdupq_n_s16(-count));
}

FORCE_INLINE Int16x8 shiftArithmetic(const Int16x8 &left, const Int16x8 &right) {
    int count = vgetq_lane_s16(right.data, 0);
    return vshlq_s16(left.data, vdupq_n_s16(-count));
}

FORCE_INLINE Int16x8 operator <<=(Int16x8 &left, const Int16x8 &right) {
    int count = vgetq_lane_s16(right.data, 0);
    left.data = vshlq_s16(left.data, vdupq_n_s16(count));
    return left;
}

FORCE_INLINE Int16x8 operator >>=(Int16x8 &left, const Int16x8 &right) {
    int count = vgetq_lane_s16(right.data, 0);
    left.data = (int16x8_t)vshlq_u16((uint16x8_t)left.data, vdupq_n_s16(-count));
    return left;
}

FORCE_INLINE Int16x8 operator <    (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(vcltq_s16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator >    (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(vcgtq_s16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator ==   (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(vceqq_s16(left.data, right.data));
}

FORCE_INLINE Int16x8 productLowerPart  (const Int16x8 &left, const Int16x8 &right) {
    return vmulq_s16(left.data, right.data);
}

FORCE_INLINE Int16x8 productHigherPart (const Int16x8 &left, const Int16x8 &right) {
    return vqdmulhq_s16(left.data, right.data);
}

FORCE_INLINE Int16x8 operator *    (const Int16x8 &left, const Int16x8 &right) {
    return productLowerPart(left, right);
}

FORCE_INLINE Int16x8 operator *=   (Int16x8 &left, const Int16x8 &right) {
    left = productLowerPart(left, right);
    return left;
}

FORCE_INLINE Int32x4 unpackLower  (const Int16x8 &left, const Int16x8 &right) {
    return (int32x4_t)vzipq_s16(left.data, right.data).val[0];
}

FORCE_INLINE Int32x4 unpackHigher (const Int16x8 &left, const Int16x8 &right) {
    return (int32x4_t)vzipq_s16(left.data, right.data).val[1];
}

FORCE_INLINE Int32x8 productExtending (const Int16x8 &left, const Int16x8 &right)
{
    Int16x8  lowParts;
    Int16x8 highParts;
    lowParts  = productLowerPart (left, right);
    highParts = productHigherPart(left, right);
    return Int32x8(unpackLower(lowParts,highParts), unpackHigher (lowParts,highParts));
}

FORCE_INLINE Int16x8 operator *  (      int16_t left, Int16x8 &right) {
    return right * Int16x8(left);
}

FORCE_INLINE Int16x8 operator *  (const Int16x8 &left, int16_t right) {
    return left * Int16x8(right);
}

FORCE_INLINE Int16x8 operator *= (Int16x8 &left, int16_t right) {
    left = left * right;
    return left;
}

} //namespace corecvs
#endif //#ifndef INT16X8_H_

