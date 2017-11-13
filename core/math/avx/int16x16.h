#ifndef INT16X16_H_
#define INT16X16_H_
/**
 * \file int16x16.h
 * \brief
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include <immintrin.h>
#include <stdint.h>

#include "core/math/avx/avxInteger.h"
#include "core/utils/global.h"
#include "core/math/vector/fixedVector.h"

namespace corecvs {


class ALIGN_DATA(32) Int16x16 : public AVXInteger<Int16x16>
{
public:  
    static const int SIZE = 16;
    static const uint32_t ALL_TOP_BITS = 0xFFFFFFFF;

    Int16x16(){}

    /**
     *  Copy constructor
     **/
    Int16x16(const Int16x16 &other) :
        AVXInteger<Int16x16>(other.data)
    {}

    /**
     *  Create SSE integer vector from integer constant
     **/
    explicit Int16x16(const __m256i &_data) {
        this->data = _data;
    }

    template<class Sibling>
    explicit Int16x16(const AVXInteger<Sibling> &other)  : AVXInteger<Int16x16>(other.data) {}


    explicit Int16x16(int16_t constant) {
        this->data = _mm256_set1_epi16(constant);
    }

    explicit Int16x16(uint16_t constant) {
         this->data = _mm256_set1_epi16(constant);
     }

    explicit Int16x16(
            int16_t  c0, int16_t  c1, int16_t  c2, int16_t  c3,
            int16_t  c4, int16_t  c5, int16_t  c6, int16_t  c7,
            int16_t  c8, int16_t  c9, int16_t c10, int16_t c11,
            int16_t c12, int16_t c13, int16_t c14, int16_t c15 ) {
        this->data = _mm256_set_epi16(
            c0, c1,  c2,  c3,  c4,  c5 , c6,  c7,
            c8, c9, c10, c11, c12, c13, c14, c15);
    }

    explicit Int16x16(const int16_t data[8]) {
        this->data = _mm256_loadu_si256((__m256i *)&data[0]);
    }

    explicit Int16x16(const uint16_t data[8]) {
        this->data = _mm256_loadu_si256((__m256i *)&data[0]);
    }

    explicit inline Int16x16(const FixedVector<int16_t,16> input)  {
        this->data = _mm256_loadu_si256((__m256i *)&input.element[0]);
    }

 /*   explicit inline Int16x16(const Int32x4 &value)
    {
        this->data = value.data;
    }*/

  /*  explicit inline Int16x16(const Int32x8 &value)
    {
        this->data =  _mm256_packs_epi32(value.element[0].data, value.element[1].data);
    }*/

    /* Static fabrics */

    static Int16x16 pack(const Int32x8 &first, const Int32x8 &second)
    {
        return Int16x16(_mm256_packs_epi32(first.data, second.data));

    }

    /** Load unaligned. */
    static Int16x16 load(const int16_t *data)
    {
        return Int16x16(data);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static Int16x16 loadAligned(const int16_t data[8])
    {
        return Int16x16(_mm256_load_si256((__m256i *)&data[0]));
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

    void save(int16_t data[16]) const
    {
        _mm256_storeu_si256((__m256i *)&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        if (((uint64_t)(void *)&data[0]) % 16)
            unalignedUWrites++;
        else
            alignedUWrites++;
#endif
    }

    void save(uint16_t data[16]) const
    {
        save((int16_t *)&data[0]);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(int16_t data[16]) const
    {
        _mm256_store_si256((__m256i *)&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
         alignedAWrites++;
#endif
    }

    void saveAligned(uint16_t data[16]) const
    {
        save((int16_t *)&data[0]);
    }

    /** Stream aligned. Not safe to use until you exactly know what you are doing */
    void streamAligned(int16_t data[8]) const
    {
        _mm256_stream_si256((__m256i *)&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        streamedWrites++;
#endif
    }

    /* converters */
template<int idx>
    uint16_t getInt() const
    {
#ifdef WIN32
        return 0xFFFF;
#else
        return _mm256_extract_epi16(this->data, idx);
#endif
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

    inline uint32_t maskToInt() const
    {
        return _mm256_movemask_epi8(this->data);
    }

    /*
    inline Int32x8 expand() const
    {
        return Int32x8(
                unpackLower4 (*this, Int16x16((uint16_t)0)),
                unpackHigher4(*this, Int16x16((uint16_t)0))
                );
    }*/

    /* Logical operations */
/*    friend Int16x16 operator &(const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator |(const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator ^(const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 andNot    (const Int16x16 &left, const Int16x16 &right);

    friend Int16x16 operator &=(Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator |=(Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator ^=(Int16x16 &left, const Int16x16 &right);
    friend Int16x16 andNotThis (Int16x16 &left, const Int16x16 &right);*/

    /* Arithmetics operations */
    friend Int16x16 operator +(const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator -(const Int16x16 &left, const Int16x16 &right);

    friend Int16x16 operator +=(Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator -=(Int16x16 &left, const Int16x16 &right);

    Int16x16 operator -( ) {
        return (Int16x16((int16_t)0) - *this);
    }

    /* Immediate shift operations */
    friend Int16x16 operator <<    (const Int16x16 &left, uint32_t count);
    friend Int16x16 operator >>    (const Int16x16 &left, uint32_t count);
    friend Int16x16 shiftArithmetic(const Int16x16 &left, uint32_t count);

    friend Int16x16 operator <<=   (Int16x16 &left, uint32_t count);
    friend Int16x16 operator >>=   (Int16x16 &left, uint32_t count);

    friend Int16x16  shiftLogical(const Int16x16 &left, uint32_t count);
           Int16x16& shiftLogical(uint32_t count);

    /* Shift operations */

    friend Int16x16 operator <<    (const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator >>    (const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 shiftArithmetic(const Int16x16 &left, const Int16x16 &right);

    friend Int16x16 operator <<=   (Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator >>=   (Int16x16 &left, const Int16x16 &right);


    /* Comparison */
    friend Int16x16 operator <    (const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator >    (const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator ==   (const Int16x16 &left, const Int16x16 &right);

    /* Multiplication beware - overrun is possible*/
    friend Int16x16 operator *    (const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 operator *=   (const Int16x16 &left, const Int16x16 &right);

    friend Int16x16 productLowerPart  (const Int16x16 &left, const Int16x16 &right);
    friend Int16x16 productHigherPart (const Int16x16 &left, const Int16x16 &right);

//    friend Int32x8 productExtending (const Int16x16 &left, const Int16x16 &right);

    /* Slow but helpful */
    friend Int16x16 operator *    (      int16_t right, Int16x16 &left);
    friend Int16x16 operator *    (const Int16x16 &left, int16_t right);
    friend Int16x16 operator *=   (      Int16x16 &left, int16_t right);

/*
    friend Int32x4 unpackLower4  (const Int16x16 &left, const Int16x16 &right);
    friend Int32x4 unpackHigher4 (const Int16x16 &left, const Int16x16 &right);
*/

    /*Print to stream helper */

    friend ostream & operator <<(ostream &out, const Int16x16 &vector);

    /* Shuffle */
    template<int imm>
    inline void shuffleLower()
    {
        this->data = _mm256_shufflelo_epi16(this->data, imm);
    }

    template<int imm>
    inline void shuffleHigher()
    {
        this->data = _mm256_shufflehi_epi16(this->data, imm);
    }

    inline void reverse()
    {
       // Was 0 1 2 3 4 5 6 7
       shuffleLower<_MM_SHUFFLE(2, 3, 0, 1)>();
       //     1 0 3 2 4 5 6 7
       shuffleHigher<_MM_SHUFFLE(2, 3, 0, 1)>();
       //     1 0 3 2 5 4 7 6
       this->data = _mm256_shuffle_epi32(this->data, _MM_SHUFFLE(0, 1, 2, 3));
       //     7 6 5 4 3 2 1 0
    }

};

FORCE_INLINE Int16x16 operator +(const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_add_epi16(left.data, right.data));
}

FORCE_INLINE Int16x16 operator -(const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_sub_epi16(left.data, right.data));
}

FORCE_INLINE Int16x16 operator +=(Int16x16 &left, const Int16x16 &right) {
    left.data = _mm256_add_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x16 operator -=(Int16x16 &left, const Int16x16 &right) {
    left.data = _mm256_sub_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x16 shiftLogical(const Int16x16 &left, uint32_t count) {
    return Int16x16(_mm256_srli_epi16(left.data, count));
}

FORCE_INLINE Int16x16 & Int16x16::shiftLogical(uint32_t count) {
    data = _mm256_srli_epi16(data, count);
    return *this;
}

FORCE_INLINE Int16x16 operator <<    (const Int16x16 &left, uint32_t count) {
    return Int16x16(_mm256_slli_epi16(left.data, count));
}

FORCE_INLINE Int16x16 operator >>    (const Int16x16 &left, uint32_t count) {
    return Int16x16(_mm256_srli_epi16(left.data, count));
}

FORCE_INLINE Int16x16 shiftArithmetic(const Int16x16 &left, uint32_t count) {
    return Int16x16(_mm256_srai_epi16(left.data, count));
}

FORCE_INLINE Int16x16 operator <<=(Int16x16 &left, uint32_t count) {
    left.data = _mm256_slli_epi16(left.data, count);
    return left;
}

FORCE_INLINE Int16x16 operator >>=(Int16x16 &left, uint32_t count) {
    left.data = _mm256_srli_epi16(left.data, count);
    return left;
}

#if 0
FORCE_INLINE Int16x16 operator <<    (const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_sll_epi16(left.data, right.data));
}

FORCE_INLINE Int16x16 operator >>    (const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_srl_epi16(left.data, right.data));
}

FORCE_INLINE Int16x16 shiftArithmetic(const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_sra_epi16(left.data, right.data));
}

FORCE_INLINE Int16x16 operator <<=(Int16x16 &left, const Int16x16 &right) {
    left.data = _mm256_sll_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x16 operator >>=(Int16x16 &left, const Int16x16 &right) {
    left.data = _mm256_srl_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x16 operator <    (const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_cmplt_epi16(left.data, right.data));
}
#endif

FORCE_INLINE Int16x16 operator >    (const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_cmpgt_epi16(left.data, right.data));
}

FORCE_INLINE Int16x16 operator ==   (const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_cmpeq_epi16(left.data, right.data));
}

FORCE_INLINE Int16x16 productLowerPart  (const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_mullo_epi16(left.data, right.data));
}

FORCE_INLINE Int16x16 productHigherPart (const Int16x16 &left, const Int16x16 &right) {
    return Int16x16(_mm256_mulhi_epi16(left.data, right.data));
}

FORCE_INLINE Int16x16 operator *    (const Int16x16 &left, const Int16x16 &right) {
    return productLowerPart(left, right);
}

FORCE_INLINE Int16x16 operator *=   (Int16x16 &left, const Int16x16 &right) {
    left = productLowerPart(left, right);
    return left;
}


FORCE_INLINE Int32x8 unpackLower4  (const Int16x16 &left, const Int16x16 &right) {
    return Int32x8(_mm256_unpacklo_epi16(left.data, right.data));
}

FORCE_INLINE Int32x8 unpackHigher4 (const Int16x16 &left, const Int16x16 &right) {
    return Int32x8(_mm256_unpackhi_epi16(left.data, right.data));
}


FORCE_INLINE Int32x16v productExtending (const Int16x16 &left, const Int16x16 &right)
{
    Int16x16  lowParts;
    Int16x16 highParts;
    lowParts  = productLowerPart (left, right);
    highParts = productHigherPart(left, right);
    return Int32x16v(unpackLower4(lowParts,highParts), unpackHigher4 (lowParts,highParts));
}

FORCE_INLINE Int16x16 operator *  (      int16_t left, Int16x16 &right) {
    return right * Int16x16(left);
}

FORCE_INLINE Int16x16 operator *  (const Int16x16 &left, int16_t right) {
    return left * Int16x16(right);
}

FORCE_INLINE Int16x16 operator *= (Int16x16 &left, int16_t right) {
    left = left * right;
    return left;
}


} //namespace corecvs
#endif //#ifndef INT16X16_H_

