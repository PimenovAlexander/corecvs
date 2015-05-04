#ifndef INT16X8_H_
#define INT16X8_H_
/**
 * \file int16x8.h
 * \brief
 *
 * \ingroup cppcorefiles
 * \date Sep 3, 2012
 * \author: a.melnikov
 */

#include <emmintrin.h>
#include <stdint.h>

#include "global.h"
#include "fixedVector.h"
#include "intBase16x8.h"

namespace corecvs {


class ALIGN_DATA(16) Int16x8 : public IntBase16x8<Int16x8>
{
public:
    typedef IntBase16x8<Int16x8> BaseClass;

    /**
     * Constructors
     **/
    Int16x8() {}

    /**
     *  Copy constructor
     **/
    Int16x8(const Int16x8 &other) : BaseClass(other) {}

    template<class Sibling>
    explicit Int16x8(const IntBase16x8<Sibling> &other)  : BaseClass(other) {}

    template<class Sibling>
    explicit Int16x8(IntBase16x8<Sibling> &other) : BaseClass(other) {}

    /**
     *  Assignment operator
     **/

    Int16x8 operator=(const BaseClass &other) {
        return Int16x8(other);
    }

    /**
     *  Create SSE integer vector from integer constant
     **/
    Int16x8(const __m128i &_data) :
            BaseClass(_data) {}

    explicit Int16x8(int16_t constant) :
            BaseClass(constant){}

   /* explicit Int16x8(uint16_t constant) :
            BaseClass(constant) {}*/

    explicit Int16x8(int16_t c0, int16_t c1, int16_t c2, int16_t c3,
                     int16_t c4, int16_t c5, int16_t c6, int16_t c7) :
            BaseClass(c0, c1, c2, c3, c4, c5, c6, c7) {}

    /* could it be data_ptr[8] ? */
    explicit Int16x8(const int16_t  * const data_ptr) :
            BaseClass(data_ptr) {}

    explicit Int16x8(const uint16_t * const data_ptr) :
            BaseClass(data_ptr) {}


    explicit inline Int16x8(const FixedVector<int16_t,8> input) :
            BaseClass(input) {}

    explicit inline Int16x8(const Int32x4 &value) :
            BaseClass(value) {}


    explicit inline Int16x8(const Int32x8 &value) :
            BaseClass(value) {}


    /* Static fabrics */
    inline Int32x8 expand() const
    {
        return Int32x8(
                Int16x8::unpackLower (*this, Int16x8((int16_t)0)),
                Int16x8::unpackHigher(*this, Int16x8((int16_t)0))
                );
    }


    Int16x8 operator -( ) const
    {
        return (Int16x8((int16_t)0) - *this);
    }

    /* Immediate shift operations */
    friend Int16x8 operator >> (const Int16x8 &left, uint32_t count);
    friend Int16x8 shiftLogical(const Int16x8 &left, uint32_t count);
    friend Int16x8 operator >>= (     Int16x8 &left, uint32_t count);
           Int16x8&shiftLogical(uint32_t count);

    /* Shift operations */
    friend Int16x8 operator >> (const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 shiftLogical(const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator >>= (     Int16x8 &left, const Int16x8 &right);

    /* === Be careful: works correctly only with positive values === */
    /* Comparison */
    friend Int16x8 operator < (const Int16x8 &left, const Int16x8 &right);
    friend Int16x8 operator > (const Int16x8 &left, const Int16x8 &right);
    /* ===                     === end ===                         === */

    /* Multiplication beware - overrun is possible*/
    friend Int16x8 productHigherPart (const Int16x8 &left, const Int16x8 &right);

    friend Int32x8 productExtending (const Int16x8 &left, const Int16x8 &right);

    /**
     *   Create a 32bit word mixing and interleaving even parts of the input
     *   0    16                                16*8
     *   | A0 | A1 | A2 | A3 | A4 | A5 | A6 | A7 |
     *   | B0 | B1 | B2 | B3 | B4 | B5 | B6 | B7 |
     *  unpacks into
     *
     *   | A0 | B0 | A1 | B1 | A2 | B2 | A3 | B3 |
     *
     **/
    inline static Int32x4 unpackLower (const Int16x8 &left, const Int16x8 &right) {
        return Int32x4(_mm_unpacklo_epi16(left.data, right.data));
    }

    /**
     *   Create a 32bit word mixing and interleaving odd parts of the input
     *   0    16                                16*8
     *   | A0 | A1 | A2 | A3 | A4 | A5 | A6 | A7 |
     *   | B0 | B1 | B2 | B3 | B4 | B5 | B6 | B7 |
     *  unpacks into
     *
     *   | A4 | B4 | A5 | B5 | A6 | B6 | A7 | B7 |
     *
     **/
    inline static Int32x4 unpackHigher (const Int16x8 &left, const Int16x8 &right) {
        return Int32x4(_mm_unpackhi_epi16(left.data, right.data));
    }

    /*Print to stream helper */
    friend ostream & operator << (ostream &out, const Int16x8 &vector);

    /* Formated hex helper */
    void hexDump (ostream &out);
};

FORCE_INLINE Int16x8 operator >> (const Int16x8 &left, uint32_t count) {
    return Int16x8(_mm_srai_epi16(left.data, count));
}

FORCE_INLINE Int16x8 shiftLogical(const Int16x8 &left, uint32_t count) {
    return Int16x8(_mm_srli_epi16(left.data, count));
}

FORCE_INLINE Int16x8 & Int16x8::shiftLogical(uint32_t count) {
    data = _mm_srli_epi16(data, count);
    return *this;
}

FORCE_INLINE Int16x8 operator >>= (Int16x8 &left, uint32_t count) {
    left.data = _mm_srai_epi16(left.data, count);
    return left;
}

FORCE_INLINE Int16x8 operator >> (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_sra_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 shiftLogical(const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_srl_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator >>= (Int16x8 &left, const Int16x8 &right) {
    left.data = _mm_sra_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE Int16x8 operator < (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_cmplt_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 operator > (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_cmpgt_epi16(left.data, right.data));
}

FORCE_INLINE Int16x8 productHigherPart (const Int16x8 &left, const Int16x8 &right) {
    return Int16x8(_mm_mulhi_epi16(left.data, right.data));
}

FORCE_INLINE Int32x8 productExtending (const Int16x8 &left, const Int16x8 &right)
{
    Int16x8  lowParts(productLowerPart (left, right));
    Int16x8 highParts(productHigherPart(left, right));

    return Int32x8(Int16x8::unpackLower (lowParts, highParts)
                 , Int16x8::unpackHigher(lowParts, highParts));
}


} //namespace corecvs

#endif //#ifndef INT16X8_H_
