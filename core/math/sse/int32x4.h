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

#include <emmintrin.h>
#include <smmintrin.h>
#include <stdint.h>

#include "global.h"
#include "fixedVector.h"
#include "int64x2.h"

namespace corecvs {

class ALIGN_DATA(16) Int32x4 : public SSEInteger<Int32x4>
{
public:
    static const int SIZE = 4;
    static const uint16_t ALL_TOP_BITS = 0xFFFF;

    typedef SSEInteger<Int32x4> SSEBase;

    /* Constructors */
    Int32x4() {}
    /**
     *  Copy constructor
     **/
    Int32x4(const Int32x4 &other) : SSEBase(other) {}

    Int32x4(const SSEBase &other) : SSEBase(other) {}

    /**
     *  Create SSE integer vector from integer constant
     **/
    Int32x4(const __m128i &_data) :
            SSEBase(_data) {}

    explicit Int32x4(int32_t constant) :
            SSEBase(_mm_set1_epi32(constant)){}

    explicit Int32x4(uint32_t constant) :
            SSEBase(_mm_set1_epi32(constant)) {}

    explicit Int32x4(int32_t c0, int32_t c1, int32_t c2, int32_t c3) :
            SSEBase(_mm_set_epi32(c3, c2, c1, c0)) {}

    explicit Int32x4(const int32_t  * const data_ptr) :
            SSEBase((__m128i *)data_ptr) {}

    explicit Int32x4(const uint32_t * const data_ptr) :
            SSEBase((__m128i *)data_ptr) {}

    explicit inline Int32x4(const FixedVector<int32_t,4> input) :
            SSEBase((__m128i *)&input.element[0]) {}

    /* Static fabrics */
    /** Load unaligned. */
    static Int32x4 load(const int32_t data[SIZE])
    {
        return SSEBase::load((__m128i *)data);
    }

    static Int32x4 load(const uint32_t data[SIZE])
    {
        return SSEBase::load((__m128i *)data);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static Int32x4 loadAligned(const int32_t data[SIZE])
    {
        return SSEBase::loadAligned((__m128i *)data);
    }

    void save(int32_t data[SIZE]) const
    {
        SSEBase::save((__m128i *)&data[0]);
    }

    void save(uint32_t data[SIZE]) const
    {
        SSEBase::save((__m128i *)&data[0]);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(int32_t data[SIZE]) const
    {
        SSEBase::saveAligned((__m128i *)&data[0]);
    }

    void saveAligned(uint32_t data[SIZE]) const
    {
        SSEBase::saveAligned((__m128i *)&data[0]);
    }

    /** Stream aligned. Not safe to use until you exactly know what you are doing */
    void streamAligned(int32_t data[SIZE]) const
    {
        SSEBase::streamAligned((__m128i *)&data[0]);
    }

    void streamAligned(uint32_t data[SIZE]) const
    {
        SSEBase::streamAligned((__m128i *)&data[0]);
    }

    // TODO: use template here <idx>.
    /* converters */
    int32_t getInt(uint32_t idx) const
    {
        CORE_ASSERT_TRUE(idx < 4, "Wrong idx in getInt()");
        ALIGN_DATA(16) int32_t result[4];
        saveAligned(result);
        return result[idx];
    }

    inline int32_t operator[] (uint32_t idx) const
    {
        return (idx < 4) ? getInt(idx) : 0;
    }

    inline uint16_t maskToInt2bit() const
    {
        return _mm_movemask_epi8(this->data);
    }

    /* Arithmetics operations */
    friend Int32x4 operator + (const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator - (const Int32x4 &left, const Int32x4 &right);

    friend Int32x4 operator += (Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator -= (Int32x4 &left, const Int32x4 &right);

    Int32x4 operator -( ) {
        return (Int32x4((int32_t)0) - *this);
    };

    /* Immediate shift operations */
    friend Int32x4 operator << (const Int32x4 &left, uint32_t count);
    friend Int32x4 operator >> (const Int32x4 &left, uint32_t count);
    friend Int32x4 shiftLogical(const Int32x4 &left, uint32_t count);

    friend Int32x4 operator <<= (Int32x4 &left, uint32_t count);
    friend Int32x4 operator >>= (Int32x4 &left, uint32_t count);
           Int32x4&shiftLogical (uint32_t count);

    /* Shift operations */
    friend Int32x4 operator << (const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator >> (const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 shiftLogical(const Int32x4 &left, const Int32x4 &right);

    friend Int32x4 operator <<= (Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator >>= (Int32x4 &left, const Int32x4 &right);

    /* Multiplication */
    friend Int32x4 operator *(const Int32x4 &left, const Int32x4 &right);
    /*friend Int32x4 operator *= (const Int32x4 &left, const Int32x4 &right);*/

    /* The division by constant*/
    friend Int32x4 operator /  (const Int32x4 &left, float divisor);
    friend Int32x4 operator /  (const Int32x4 &left, int divisor);
    friend Int32x4 operator /= (Int32x4 &left, float divisor);
    friend Int32x4 operator /= (Int32x4 &left, int divisor);

    /* Comparison */
    friend Int32x4 operator <  (const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator >  (const Int32x4 &left, const Int32x4 &right);
    friend Int32x4 operator == (const Int32x4 &left, const Int32x4 &right);

    /* Shuffle */
    template<int imm>
    inline void shuffle()
    {
        this->data = _mm_shuffle_epi32(this->data, imm);
    }

    template<int imm>
    inline Int32x4 shuffled() const
    {
        return Int32x4(_mm_shuffle_epi32(this->data, imm));
    }

    inline void reverse()
    {
        shuffle<_MM_SHUFFLE(3, 2, 1, 0)>();
    }

    inline Int32x4 reversed() const
    {
        return shuffled<_MM_SHUFFLE(3, 2, 1, 0)>();
    }

    /* Print to stream helper */
    friend ostream & operator << (ostream &out, const Int32x4 &vector);

    /* Formated hex helper */
    void hexDump (ostream &out);

    // Producers
    inline static Int64x2 unpackLower(const Int32x4 &left, const Int32x4 &right) {
        return Int64x2(_mm_unpacklo_epi32(left.data, right.data));
    }

    inline static Int64x2 unpackHigher(const Int32x4 &left, const Int32x4 &right) {
        return Int64x2(_mm_unpackhi_epi32(left.data, right.data));
    }

};

FORCE_INLINE Int32x4 operator + (const Int32x4 &left, const Int32x4 &right) {
    return Int32x4(_mm_add_epi32(left.data, right.data));
}

FORCE_INLINE Int32x4 operator - (const Int32x4 &left, const Int32x4 &right) {
    return Int32x4(_mm_sub_epi32(left.data, right.data));
}

FORCE_INLINE Int32x4 operator += (Int32x4 &left, const Int32x4 &right) {
    left.data = _mm_add_epi32(left.data, right.data);
    return left;
}

FORCE_INLINE Int32x4 operator -= (Int32x4 &left, const Int32x4 &right) {
    left.data = _mm_sub_epi32(left.data, right.data);
    return left;
}

/* Multiplication */
/* Depending on your platform this can be slow */
FORCE_INLINE Int32x4 operator * (const Int32x4 &left, const Int32x4 &right) {
#ifdef __SSE4_1__
    return Int32x4(_mm_mullo_epi32(left.data, right.data));
#else
    Int32x4 tmp1(_mm_mul_epu32( left.data, right.data));                                           /* mul 2,0 */
    Int32x4 tmp2(_mm_mul_epu32( left.shiftRightWhole<4>().data, right.shiftRightWhole<4>().data)); /* mul 3,1 */

    tmp1.shuffle<_MM_SHUFFLE (0,0,2,0)>();
    tmp2.shuffle<_MM_SHUFFLE (0,0,2,0)>();

    /* shuffle results to [63..0] and pack */
    return Int32x4(Int32x4::unpackLower(tmp1, tmp2).data);
#endif
}
/*
FORCE_INLINE Int32x4 operator *= (Int32x4 &left, const Int32x4 &right) {
    left.data = _mm_mul_epu32(left.data, right.data);
    return left;
}
*/

/*FORCE_INLINE Int32x4 productLower (const Int32x4 &left, const Int32x4 &right) {
    return Int32x4(_mm_mul_epu16(left.data, right.data));
}*/

/* Shifts */
FORCE_INLINE Int32x4 operator << (const Int32x4 &left, uint32_t count) {
    return Int32x4(_mm_slli_epi32(left.data, count));
}

FORCE_INLINE Int32x4 operator >> (const Int32x4 &left, uint32_t count) {
    return Int32x4(_mm_srai_epi32(left.data, count));
}

FORCE_INLINE Int32x4 shiftLogical(const Int32x4 &left, uint32_t count) {
    return Int32x4(_mm_srli_epi32(left.data, count));
}

FORCE_INLINE Int32x4 & Int32x4::shiftLogical(uint32_t count) {
    data = _mm_srli_epi32(data, count);
    return *this;
}

FORCE_INLINE Int32x4 operator <<= (Int32x4 &left, uint32_t count) {
    left.data = _mm_slli_epi32(left.data, count);
    return left;
}

FORCE_INLINE Int32x4 operator >>= (Int32x4 &left, uint32_t count) {
    left.data = _mm_srai_epi32(left.data, count);
    return left;
}

FORCE_INLINE Int32x4 operator << (const Int32x4 &left, const Int32x4 &right) {
    return Int32x4(_mm_sll_epi32(left.data, right.data));
}

FORCE_INLINE Int32x4 operator >> (const Int32x4 &left, const Int32x4 &right) {
    return Int32x4(_mm_sra_epi32(left.data, right.data));
}

FORCE_INLINE Int32x4 shiftLogical(const Int32x4 &left, const Int32x4 &right) {
    return Int32x4(_mm_srl_epi32(left.data, right.data));
}

FORCE_INLINE Int32x4 operator <<= (Int32x4 &left, const Int32x4 &right) {
    left.data = _mm_sll_epi32(left.data, right.data);
    return left;
}

FORCE_INLINE Int32x4 operator >>= (Int32x4 &left, const Int32x4 &right) {
    left.data = _mm_sra_epi32(left.data, right.data);
    return left;
}

FORCE_INLINE Int32x4 operator < (const Int32x4 &left, const Int32x4 &right) {
    return Int32x4(_mm_cmplt_epi32(left.data, right.data));
}

FORCE_INLINE Int32x4 operator > (const Int32x4 &left, const Int32x4 &right) {
    return Int32x4(_mm_cmpgt_epi32(left.data, right.data));
}

FORCE_INLINE Int32x4 operator == (const Int32x4 &left, const Int32x4 &right) {
    return Int32x4(_mm_cmpeq_epi32(left.data, right.data));
}


} //namespace corecvs

#endif  //INT32X4_H_
