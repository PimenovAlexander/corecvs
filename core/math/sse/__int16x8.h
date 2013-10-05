#ifdef NOT_DEF

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

#include <emmintrin.h>
#include <stdint.h>

#include "global.h"
#include "fixedVector.h"

namespace corecvs {


class ALIGN_DATA(16) Int16x8 : public SSEInteger<Int16x8>
{
public:
    typedef SSEInteger<Int16x8> SSEBase;

    /* Constructors */

    Int16x8(){}

    /**
     *  Copy constructor
     **/
    Int16x8(const Int16x8 &other) : SSEBase(other) {}

    explicit Int16x8(const SSEBase &other) : SSEBase(other) {}


    /**
     *  Create SSE integer vector from integer constant
     **/
    Int16x8(const __m128i &_data) :
            SSEBase(_data) {}

    explicit Int16x8(int16_t constant) :
            SSEBase(_mm_set1_epi16(constant)){}

    explicit Int16x8(uint16_t constant) :
            SSEBase(_mm_set1_epi16(constant)) {}

    explicit Int16x8(int16_t c0, int16_t c1, int16_t c2, int16_t c3,
                     int16_t c4, int16_t c5, int16_t c6, int16_t c7) :
            SSEBase(_mm_set_epi16(c0, c1, c2, c3, c4, c5, c6, c7)) {}

    explicit Int16x8(const int16_t  * const data_ptr) :
            SSEBase((__m128i *)data_ptr) {}

    explicit Int16x8(const uint16_t * const data_ptr) :
            SSEBase((__m128i *)data_ptr) {}


    explicit inline Int16x8(const FixedVector<int16_t,8> input) :
            SSEBase((__m128i *)&input.element[0]) {}

    explicit inline Int16x8(const Int32x4 &value) :
            SSEBase(value.data) {}


    explicit inline Int16x8(const Int32x8 &value) :
            SSEBase(
                _mm_packs_epi32(
                    value.element[0].data,
                    value.element[1].data
                )
            ) {}


    /* Static fabrics */

    /**
     *   This functions packs the lower parts of each of elements of input registers into one.
     *   Putting one after the other.
     **/
    static Int16x8 pack(const Int32x4 &first, const Int32x4 &second)
    {
        return Int16x8(_mm_packs_epi32(first.data, second.data));
    }

    /** Load unaligned. Not safe to use until you exactly know what you are doing */
    static Int16x8 load(const int16_t data[8])
    {
        return Int16x8(SSEBase::load((__m128i *)data));
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static Int16x8 loadAligned(const int16_t data[8])
    {
        return Int16x8(SSEBase::loadAligned((__m128i *)data));
    }

    void save(int16_t data[8]) const
    {
        SSEBase::save((__m128i *)&data[0]);
    }

    void save(uint16_t data[8]) const
    {
        SSEBase::save((__m128i *)&data[0]);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(int16_t data[8]) const
    {
        SSEBase::saveAligned((__m128i *)&data[0]);
    }

    void saveAligned(uint16_t data[8]) const
    {
        SSEBase::saveAligned((__m128i *)&data[0]);
    }

    /** Stream aligned. Not safe to use until you exactly know what you are doing */
    void streamAligned(int16_t data[8]) const
    {
        SSEBase::streamAligned((__m128i *)&data[0]);
    }

    /* Reads the lower 64 bits of b and stores them into the lower 64 bits of a.  */
    void saveLower(uint16_t data[8]) const
    {
        _mm_storel_epi64((__m128i *)&data[0], this->data);
    }

    /* converters */
template<int idx>
    uint16_t getInt() const
    {
        return _mm_extract_epi16(this->data, idx);
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
        return _mm_movemask_epi8(this->data);
    }

    inline Int32x8 expand() const
    {
        return Int32x8(
                Int16x8::unpackLower (*this, Int16x8((uint16_t)0)),
                Int16x8::unpackHigher(*this, Int16x8((uint16_t)0))
                );
    }


    /* Arithmetics operations */
    friend Int16x8 operator +(const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator -(const Int16x8 &left, const Int16x8 &right);

    friend Int16x8 operator +=(Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator -=(Int16x8 &left, const Int16x8 &right);

    Int16x8 operator -( ) {
        return (Int16x8((int16_t)0) - *this);
    };

    /* Immediate shift operations */
    friend Int16x8 operator <<    (const Int16x8 &left, uint32_t count);
    friend Int16x8 operator >>    (const Int16x8 &left, uint32_t count);
    friend Int16x8 shiftArithmetic(const Int16x8 &left, uint32_t count);

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

    friend Int32x8 productExtending (const Int16x8 &left, const Int16x8 &right);

    /* Slow but helpful */
    friend Int16x8 operator *    (      int16_t right, const Int16x8 &left);
    friend Int16x8 operator *    (const Int16x8 &left, int16_t right);
    friend Int16x8 operator *=   (      Int16x8 &left, int16_t right);

    /*Print to stream helper */

    friend ostream & operator <<(ostream &out, const Int16x8 &vector);

    /* Shuffle */
    template<int imm>
    inline void shuffleLower()
    {
        this->data = _mm_shufflelo_epi16(this->data, imm);
    }

    template<int imm>
    inline void shuffleHigher()
    {
        this->data = _mm_shufflehi_epi16(this->data, imm);
    }

    inline void reverse()
    {
       // Was 0 1 2 3 4 5 6 7
       shuffleLower<_MM_SHUFFLE(2, 3, 0, 1)>();
       //     1 0 3 2 4 5 6 7
       shuffleHigher<_MM_SHUFFLE(2, 3, 0, 1)>();
       //     1 0 3 2 5 4 7 6
       this->data = _mm_shuffle_epi32(this->data, _MM_SHUFFLE(0, 1, 2, 3));
       //     7 6 5 4 3 2 1 0
    }

    template<int imm>
    inline Int16x8 shuffled() const
    {
        return Int16x8(_mm_shuffle_epi32(this->data, imm));
    }


    // Producers
    inline static Int32x4 unpackLower  (const Int16x8 &left, const Int16x8 &right) {
        return Int32x4(_mm_unpacklo_epi16(left.data, right.data));
    }

    inline static Int32x4 unpackHigher (const Int16x8 &left, const Int16x8 &right) {
        return Int32x4(_mm_unpackhi_epi16(left.data, right.data));
    }


};


FORCE_INLINE Int16x8 operator +(const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_add_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator -(const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_sub_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator +=(Int16x8 &left, const Int16x8 &right) {
    left.data = _mm_add_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 operator -=(Int16x8 &left, const Int16x8 &right) {
    left.data = _mm_sub_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 operator <<    (const Int16x8 &left, uint32_t count) {
    return Int16x8(_mm_slli_epi16(left.data, count));
}

FORCE_INLINE Int16x8 operator >>    (const Int16x8 &left, uint32_t count) {
    return Int16x8(_mm_srli_epi16(left.data, count));
}

FORCE_INLINE Int16x8 shiftArithmetic(const Int16x8 &left, uint32_t count) {
    return Int16x8(_mm_srai_epi16(left.data, count));
}

FORCE_INLINE Int16x8 operator <<=(Int16x8 &left, uint32_t count) {
    left.data = _mm_slli_epi16(left.data, count);
    return left;
}

FORCE_INLINE Int16x8 operator >>=(Int16x8 &left, uint32_t count) {
    left.data = _mm_srli_epi16(left.data, count);
    return left;
}

FORCE_INLINE Int16x8 operator <<    (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_sll_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator >>    (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_srl_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 shiftArithmetic(const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_sra_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator <<=(Int16x8 &left, const Int16x8 &right) {
    left.data = _mm_sll_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 operator >>=(Int16x8 &left, const Int16x8 &right) {
    left.data = _mm_srl_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 operator <    (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_cmplt_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator >    (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_cmpgt_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator ==   (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_cmpeq_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 productLowerPart  (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_mullo_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 productHigherPart (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_mulhi_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator *    (const Int16x8 &left, const Int16x8 &right) {
    return productLowerPart(left, right);
}

FORCE_INLINE Int16x8 operator *=   (Int16x8 &left, const Int16x8 &right) {
    left = productLowerPart(left, right);
    return left;
}

FORCE_INLINE Int32x8 productExtending (const Int16x8 &left, const Int16x8 &right)
{
    Int16x8  lowParts;
    Int16x8 highParts;
    lowParts  = productLowerPart (left, right);
    highParts = productHigherPart(left, right);
    return Int32x8(Int16x8::unpackLower(lowParts,highParts), Int16x8::unpackHigher(lowParts,highParts));
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

#endif
