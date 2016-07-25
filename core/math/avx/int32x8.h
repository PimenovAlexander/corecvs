#ifndef INT32X8_H_
#define INT32X8_H_
/**
 * \file Int32x8.h
 * \brief a header for Int32x8.c
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include <immintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <stdint.h>

#include "avxInteger.h"
#include "global.h"
#include "fixedVector.h"
#include "int64x2.h"

namespace corecvs {

class ALIGN_DATA(32) Int32x8 : public AVXInteger<Int32x8>
{
public:
    static const int SIZE = 8;

    static const uint32_t ALL_TOP_BITS = 0xFFFFFFFF;

    typedef AVXInteger<Int32x8> SSEBase;

    /* Constructors */
    Int32x8() {}
    /**
     *  Copy constructor
     **/
    Int32x8(const Int32x8 &other) : SSEBase(other) {}

    Int32x8(const SSEBase &other) : SSEBase(other) {}

    /**
     *  Create SSE integer vector from integer constant
     **/
    Int32x8(const __m256i &_data) :
            SSEBase(_data) {}

    explicit Int32x8(int32_t constant) :
            SSEBase(_mm256_set1_epi32(constant)){}

    explicit Int32x8(uint32_t constant) :
            SSEBase(_mm256_set1_epi32(constant)) {}

    explicit Int32x8(int32_t c0, int32_t c1, int32_t c2, int32_t c3,
                     int32_t c4, int32_t c5, int32_t c6, int32_t c7
                     ) :
            SSEBase(_mm256_set_epi32(c7, c6, c5, c4, c3, c2, c1, c0)) {}

    explicit Int32x8(const int32_t  * const data_ptr) :
            SSEBase((__m256i *)data_ptr) {}

    explicit Int32x8(const uint32_t * const data_ptr) :
            SSEBase((__m256i *)data_ptr) {}

    explicit inline Int32x8(const FixedVector<int32_t,8> input) :
            SSEBase((__m256i *)&input.element[0]) {}

    /* Static fabrics */
    /** Load unaligned. */
    static Int32x8 load(const int32_t data[SIZE])
    {
        return SSEBase::load((__m256i *)data);
    }

    static Int32x8 load(const uint32_t data[SIZE])
    {
        return SSEBase::load((__m256i *)data);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static Int32x8 loadAligned(const int32_t data[8])
    {
        return SSEBase::loadAligned((__m256i *)data);
    }

    void save(int32_t data[SIZE]) const
    {
        SSEBase::save((__m256i *)&data[0]);
    }

    void save(uint32_t data[SIZE]) const
    {
        SSEBase::save((__m256i *)&data[0]);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(int32_t data[SIZE]) const
    {
        SSEBase::saveAligned((__m256i *)&data[0]);
    }

    void saveAligned(uint32_t data[SIZE]) const
    {
        SSEBase::saveAligned((__m256i *)&data[0]);
    }

    /** Stream aligned. Not safe to use until you exactly know what you are doing */
    void streamAligned(int32_t data[SIZE]) const
    {
        SSEBase::streamAligned((__m256i *)&data[0]);
    }

    void streamAligned(uint32_t data[SIZE]) const
    {
        SSEBase::streamAligned((__m256i *)&data[0]);
    }

    // TODO: use template here <idx>.
    /* converters */
    int32_t getInt(uint32_t idx) const
    {
        CORE_ASSERT_TRUE(idx < SIZE, "Wrong idx in getInt()");
        ALIGN_DATA(16) int32_t result[SIZE];
        saveAligned(result);
        return result[idx];
    }

    inline int32_t operator[] (uint32_t idx) const
    {
        return (idx < SIZE) ? getInt(idx) : 0;
    }

    inline uint32_t maskToInt2bit() const
    {
        return _mm256_movemask_epi8(this->data);
    }

    /* Arithmetics operations */
    friend Int32x8 operator + (const Int32x8 &left, const Int32x8 &right);
    friend Int32x8 operator - (const Int32x8 &left, const Int32x8 &right);

    friend Int32x8 operator += (Int32x8 &left, const Int32x8 &right);
    friend Int32x8 operator -= (Int32x8 &left, const Int32x8 &right);

    Int32x8 operator -( ) {
        return (Int32x8((int32_t)0) - *this);
    }

    /* Immediate shift operations */
    friend Int32x8 operator << (const Int32x8 &left, uint32_t count);
    friend Int32x8 operator >> (const Int32x8 &left, uint32_t count);
    friend Int32x8 shiftLogical(const Int32x8 &left, uint32_t count);

    friend Int32x8 operator <<= (Int32x8 &left, uint32_t count);
    friend Int32x8 operator >>= (Int32x8 &left, uint32_t count);
           Int32x8&shiftLogical (uint32_t count);

    /* Shift operations */
    friend Int32x8 operator << (const Int32x8 &left, const Int32x8 &right);
    friend Int32x8 operator >> (const Int32x8 &left, const Int32x8 &right);
    friend Int32x8 shiftLogical(const Int32x8 &left, const Int32x8 &right);

    friend Int32x8 operator <<= (Int32x8 &left, const Int32x8 &right);
    friend Int32x8 operator >>= (Int32x8 &left, const Int32x8 &right);

    /* Multiplication */
    friend Int32x8 operator *(const Int32x8 &left, const Int32x8 &right);
    /*friend Int32x8 operator *= (const Int32x8 &left, const Int32x8 &right);*/

    /* The division by constant*/
    friend Int32x8 operator /  (const Int32x8 &left, float divisor);
    friend Int32x8 operator /  (const Int32x8 &left, int divisor);
    friend Int32x8 operator /= (Int32x8 &left, float divisor);
    friend Int32x8 operator /= (Int32x8 &left, int divisor);

    /* Comparison */
    friend Int32x8 operator <  (const Int32x8 &left, const Int32x8 &right);
    friend Int32x8 operator >  (const Int32x8 &left, const Int32x8 &right);
    friend Int32x8 operator == (const Int32x8 &left, const Int32x8 &right);

    /* Shuffle */
#if 0
    template<int imm>
    inline void shuffle()
    {
        this->data = _mm256_shuffle_epi32(this->data, imm);
    }

    template<int imm>
    inline Int32x8 shuffled() const
    {
        return Int32x8(_mm256_shuffle_epi32(this->data, imm));
    }

    inline void reverse()
    {
        shuffle<_MM_SHUFFLE(3, 2, 1, 0)>();
    }

    inline Int32x8 reversed() const
    {
        return shuffled<_MM_SHUFFLE(3, 2, 1, 0)>();
    }
#endif

    /* Print to stream helper */
    friend ostream & operator << (ostream &out, const Int32x8 &vector);

    /* Formated hex helper */
    void hexDump (ostream &out);
#if 1
    // Producers
    inline static Int64x4 unpackLower(const Int32x8 &left, const Int32x8 &right) {
        return Int64x4(_mm256_unpacklo_epi32(left.data, right.data));
    }

    inline static Int64x4 unpackHigher(const Int32x8 &left, const Int32x8 &right) {
        return Int64x4(_mm256_unpackhi_epi32(left.data, right.data));
    }
#endif

};

FORCE_INLINE Int32x8 operator + (const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm256_add_epi32(left.data, right.data));
}

FORCE_INLINE Int32x8 operator - (const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm256_sub_epi32(left.data, right.data));
}

FORCE_INLINE Int32x8 operator += (Int32x8 &left, const Int32x8 &right) {
    left.data = _mm256_add_epi32(left.data, right.data);
    return left;
}

FORCE_INLINE Int32x8 operator -= (Int32x8 &left, const Int32x8 &right) {
    left.data = _mm256_sub_epi32(left.data, right.data);
    return left;
}

/* Multiplication */
FORCE_INLINE Int32x8 operator * (const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm256_mullo_epi32(left.data, right.data));
}
/*
FORCE_INLINE Int32x8 operator *= (Int32x8 &left, const Int32x8 &right) {
    left.data = _mm_mul_epu32(left.data, right.data);
    return left;
}
*/

/*FORCE_INLINE Int32x8 productLower (const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm_mul_epu16(left.data, right.data));
}*/

/* Shifts */
FORCE_INLINE Int32x8 operator << (const Int32x8 &left, uint32_t count) {
    return Int32x8(_mm256_slli_epi32(left.data, count));
}

FORCE_INLINE Int32x8 operator >> (const Int32x8 &left, uint32_t count) {
    return Int32x8(_mm256_srai_epi32(left.data, count));
}

FORCE_INLINE Int32x8 shiftLogical(const Int32x8 &left, uint32_t count) {
    return Int32x8(_mm256_srli_epi32(left.data, count));
}

FORCE_INLINE Int32x8 & Int32x8::shiftLogical(uint32_t count) {
    data = _mm256_srli_epi32(data, count);
    return *this;
}

FORCE_INLINE Int32x8 operator <<= (Int32x8 &left, uint32_t count) {
    left.data = _mm256_slli_epi32(left.data, count);
    return left;
}

FORCE_INLINE Int32x8 operator >>= (Int32x8 &left, uint32_t count) {
    left.data = _mm256_srai_epi32(left.data, count);
    return left;
}

#if 0
FORCE_INLINE Int32x8 operator << (const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm256_sll_epi32(left.data, right.data));
}

FORCE_INLINE Int32x8 operator >> (const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm256_sra_epi32(left.data, right.data));
}

FORCE_INLINE Int32x8 shiftLogical(const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm256_srl_epi32(left.data, right.data));
}

FORCE_INLINE Int32x8 operator <<= (Int32x8 &left, const Int32x8 &right) {
    left.data = _mm256_sll_epi32(left.data, right.data);
    return left;
}

FORCE_INLINE Int32x8 operator >>= (Int32x8 &left, const Int32x8 &right) {
    left.data = _mm256_sra_epi32(left.data, right.data);
    return left;
}
#endif

FORCE_INLINE Int32x8 operator < (const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm256_cmpgt_epi32(right.data, left.data));
}

FORCE_INLINE Int32x8 operator > (const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm256_cmpgt_epi32(left.data, right.data));
}

FORCE_INLINE Int32x8 operator == (const Int32x8 &left, const Int32x8 &right) {
    return Int32x8(_mm256_cmpeq_epi32(left.data, right.data));
}


} //namespace corecvs

#endif  //INT32X8_H_
