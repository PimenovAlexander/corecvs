#ifndef INT64X4_H_
#define INT64X4_H_
/**
 * \file Int64x4.h
 * \brief a header for Int64x4.c
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

class ALIGN_DATA(32) Int64x4 : public AVXInteger<Int64x4>
{
public:
    static const int SIZE = 4;

    typedef AVXInteger<Int64x4> SSEBase;

    /* Constructors */
    Int64x4() {}
    /**
     *  Copy constructor
     **/
    Int64x4(const Int64x4 &other) : SSEBase(other) {}

    Int64x4(const SSEBase &other) : SSEBase(other) {}

    /**
     *  Create SSE integer vector from integer constant
     **/
    Int64x4(const __m256i &_data) :
            SSEBase(_data) {}

    explicit Int64x4(int64_t constant) :
            SSEBase(_mm256_set1_epi64x(constant)){}

    explicit Int64x4(uint64_t constant) :
            SSEBase(_mm256_set1_epi64x(constant)) {}

    explicit Int64x4(int64_t c0, int64_t c1, int64_t c2, int64_t c3) :
            SSEBase(_mm256_set_epi64x(c3, c2, c1, c0)) {}

    explicit Int64x4(const int64_t  * const data_ptr) :
            SSEBase((__m256i *)data_ptr) {}

    explicit Int64x4(const uint64_t * const data_ptr) :
            SSEBase((__m256i *)data_ptr) {}

    explicit inline Int64x4(const FixedVector<int64_t,8> input) :
            SSEBase((__m256i *)&input.element[0]) {}

    /* Static fabrics */
    /** Load unaligned. */
    static Int64x4 load(const int64_t data[SIZE])
    {
        return SSEBase::load((__m256i *)data);
    }

    static Int64x4 load(const uint64_t data[SIZE])
    {
        return SSEBase::load((__m256i *)data);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static Int64x4 loadAligned(const int64_t data[8])
    {
        return SSEBase::loadAligned((__m256i *)data);
    }

    void save(int64_t data[SIZE]) const
    {
        SSEBase::save((__m256i *)&data[0]);
    }

    void save(uint64_t data[SIZE]) const
    {
        SSEBase::save((__m256i *)&data[0]);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(int64_t data[SIZE]) const
    {
        SSEBase::saveAligned((__m256i *)&data[0]);
    }

    void saveAligned(uint64_t data[SIZE]) const
    {
        SSEBase::saveAligned((__m256i *)&data[0]);
    }

    /** Stream aligned. Not safe to use until you exactly know what you are doing */
    void streamAligned(int64_t data[SIZE]) const
    {
        SSEBase::streamAligned((__m256i *)&data[0]);
    }

    void streamAligned(uint64_t data[SIZE]) const
    {
        SSEBase::streamAligned((__m256i *)&data[0]);
    }

    // TODO: use template here <idx>.
    /* converters */
    int64_t getInt(uint64_t idx) const
    {
        CORE_ASSERT_TRUE(idx < SIZE, "Wrong idx in getInt()");
        ALIGN_DATA(16) int64_t result[SIZE];
        saveAligned(result);
        return result[idx];
    }

    inline int64_t operator[] (uint64_t idx) const
    {
        return (idx < SIZE) ? getInt(idx) : 0;
    }

    inline uint16_t maskToInt2bit() const
    {
        return _mm256_movemask_epi8(this->data);
    }

    /* Arithmetics operations */
    friend Int64x4 operator + (const Int64x4 &left, const Int64x4 &right);
    friend Int64x4 operator - (const Int64x4 &left, const Int64x4 &right);

    friend Int64x4 operator += (Int64x4 &left, const Int64x4 &right);
    friend Int64x4 operator -= (Int64x4 &left, const Int64x4 &right);

    Int64x4 operator -( ) {
        return (Int64x4((int64_t)0) - *this);
    }

    /* Immediate shift operations */
    friend Int64x4 operator << (const Int64x4 &left, uint64_t count);
    friend Int64x4 operator >> (const Int64x4 &left, uint64_t count);
    friend Int64x4 shiftLogical(const Int64x4 &left, uint64_t count);

    friend Int64x4 operator <<= (Int64x4 &left, uint64_t count);
    friend Int64x4 operator >>= (Int64x4 &left, uint64_t count);
           Int64x4&shiftLogical (uint64_t count);

    /* Shift operations */
    friend Int64x4 operator << (const Int64x4 &left, const Int64x4 &right);
    friend Int64x4 operator >> (const Int64x4 &left, const Int64x4 &right);
    friend Int64x4 shiftLogical(const Int64x4 &left, const Int64x4 &right);

    friend Int64x4 operator <<= (Int64x4 &left, const Int64x4 &right);
    friend Int64x4 operator >>= (Int64x4 &left, const Int64x4 &right);

    /* Multiplication */
    friend Int64x4 operator *(const Int64x4 &left, const Int64x4 &right);
    /*friend Int64x4 operator *= (const Int64x4 &left, const Int64x4 &right);*/

    /* The division by constant*/
    friend Int64x4 operator /  (const Int64x4 &left, float divisor);
    friend Int64x4 operator /  (const Int64x4 &left, int divisor);
    friend Int64x4 operator /= (Int64x4 &left, float divisor);
    friend Int64x4 operator /= (Int64x4 &left, int divisor);

    /* Comparison */
    friend Int64x4 operator <  (const Int64x4 &left, const Int64x4 &right);
    friend Int64x4 operator >  (const Int64x4 &left, const Int64x4 &right);
    friend Int64x4 operator == (const Int64x4 &left, const Int64x4 &right);

    /* Shuffle */
#if 0
    template<int imm>
    inline void shuffle()
    {
        this->data = _mm256_shuffle_epi64(this->data, imm);
    }

    template<int imm>
    inline Int64x4 shuffled() const
    {
        return Int64x4(_mm256_shuffle_epi64(this->data, imm));
    }

    inline void reverse()
    {
        shuffle<_MM_SHUFFLE(3, 2, 1, 0)>();
    }

    inline Int64x4 reversed() const
    {
        return shuffled<_MM_SHUFFLE(3, 2, 1, 0)>();
    }
#endif

    /* Print to stream helper */
    friend ostream & operator << (ostream &out, const Int64x4 &vector);

    /* Formated hex helper */
    void hexDump (ostream &out);
#if 1
    // Producers
    inline static Int64x4 unpackLower(const Int64x4 &left, const Int64x4 &right) {
        return Int64x4(_mm256_unpacklo_epi64(left.data, right.data));
    }

    inline static Int64x4 unpackHigher(const Int64x4 &left, const Int64x4 &right) {
        return Int64x4(_mm256_unpackhi_epi64(left.data, right.data));
    }
#endif

};

FORCE_INLINE Int64x4 operator + (const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm256_add_epi64(left.data, right.data));
}

FORCE_INLINE Int64x4 operator - (const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm256_sub_epi64(left.data, right.data));
}

FORCE_INLINE Int64x4 operator += (Int64x4 &left, const Int64x4 &right) {
    left.data = _mm256_add_epi64(left.data, right.data);
    return left;
}

FORCE_INLINE Int64x4 operator -= (Int64x4 &left, const Int64x4 &right) {
    left.data = _mm256_sub_epi64(left.data, right.data);
    return left;
}

/* Multiplication */
/*
FORCE_INLINE Int64x4 operator * (const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm256_mullo_epi64(left.data, right.data));
}
*/
/*
FORCE_INLINE Int64x4 operator *= (Int64x4 &left, const Int64x4 &right) {
    left.data = _mm_mul_epu32(left.data, right.data);
    return left;
}
*/

/*FORCE_INLINE Int64x4 productLower (const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm_mul_epu16(left.data, right.data));
}*/

/* Shifts */
FORCE_INLINE Int64x4 operator << (const Int64x4 &left, uint64_t count) {
    return Int64x4(_mm256_slli_epi64(left.data, count));
}

/*FORCE_INLINE Int64x4 operator >> (const Int64x4 &left, uint64_t count) {
    return Int64x4(_mm256_srai_epi64(left.data, count));
}*/

FORCE_INLINE Int64x4 shiftLogical(const Int64x4 &left, uint64_t count) {
    return Int64x4(_mm256_srli_epi64(left.data, count));
}

FORCE_INLINE Int64x4 & Int64x4::shiftLogical(uint64_t count) {
    data = _mm256_srli_epi64(data, count);
    return *this;
}

FORCE_INLINE Int64x4 operator <<= (Int64x4 &left, uint64_t count) {
    left.data = _mm256_slli_epi64(left.data, count);
    return left;
}

/*FORCE_INLINE Int64x4 operator >>= (Int64x4 &left, uint64_t count) {
    left.data = _mm256_srai_epi64(left.data, count);
    return left;
}*/

#if 0
FORCE_INLINE Int64x4 operator << (const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm256_sll_epi64(left.data, right.data));
}

FORCE_INLINE Int64x4 operator >> (const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm256_sra_epi64(left.data, right.data));
}

FORCE_INLINE Int64x4 shiftLogical(const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm256_srl_epi64(left.data, right.data));
}

FORCE_INLINE Int64x4 operator <<= (Int64x4 &left, const Int64x4 &right) {
    left.data = _mm256_sll_epi64(left.data, right.data);
    return left;
}

FORCE_INLINE Int64x4 operator >>= (Int64x4 &left, const Int64x4 &right) {
    left.data = _mm256_sra_epi64(left.data, right.data);
    return left;
}
#endif

FORCE_INLINE Int64x4 operator < (const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm256_cmpgt_epi64(right.data, left.data));
}

FORCE_INLINE Int64x4 operator > (const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm256_cmpgt_epi64(left.data, right.data));
}

FORCE_INLINE Int64x4 operator == (const Int64x4 &left, const Int64x4 &right) {
    return Int64x4(_mm256_cmpeq_epi64(left.data, right.data));
}


} //namespace corecvs

#endif  //INT64X4_H_
