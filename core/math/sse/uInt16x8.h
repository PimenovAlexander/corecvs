#ifndef UINT16X8_H
#define UINT16X8_H

/**
 * \file uInt16x8.h
 * \brief
 *
 * \ingroup cppcorefiles
 * \date Sept 2, 2012
 * \author: apimenov
 */

#include <emmintrin.h>
#include <stdint.h>

#include "core/utils/global.h"

#include "core/math/vector/fixedVector.h"
#include "core/math/sse/intBase16x8.h"

namespace corecvs {


class ALIGN_DATA(16) UInt16x8 : public IntBase16x8<UInt16x8>
{
public:
    /* Shortcut for the base type */
    typedef IntBase16x8<UInt16x8> BaseClass;

    /* Constructors */
    UInt16x8() {}

    /**
     *  Copy constructor
     **/
    UInt16x8(const UInt16x8 &other) : BaseClass(other) {}

   // explicit UInt16x8(const BaseClass &other) : BaseClass(other) {}

    template<class Sibling>
    explicit UInt16x8(const IntBase16x8<Sibling> &other)  : BaseClass(other) {}

    template<class Sibling>
    explicit UInt16x8(IntBase16x8<Sibling> &other) : BaseClass(other) {}



    /**
     *  Create SSE integer vector from integer constant
     **/
    UInt16x8(const __m128i &_data) :
            BaseClass(_data) {}

    explicit UInt16x8(uint16_t constant) :
            BaseClass(int16_t(constant)){}

    explicit UInt16x8(uint16_t c0, uint16_t c1, uint16_t c2, uint16_t c3,
                      uint16_t c4, uint16_t c5, uint16_t c6, uint16_t c7) :
            BaseClass(int16_t(c0), int16_t(c1), int16_t(c2), int16_t(c3), int16_t(c4), int16_t(c5), int16_t(c6), int16_t(c7)) {}

    explicit UInt16x8(const uint16_t  * const data_ptr) :
            BaseClass((const int16_t* )data_ptr) {}


    explicit inline UInt16x8(const FixedVector<uint16_t,8> input) :
            BaseClass((const int16_t*) &(input[0])) {}

    /** Load unaligned. Not safe to use until you exactly know what you are doing */
    static UInt16x8 load(const uint16_t data[8])
    {
        return BaseClass::load((const int16_t*) data);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static UInt16x8 loadAligned(const uint16_t data[8])
    {
        return BaseClass::loadAligned((const int16_t*) data);
    }

    void save(uint16_t data[8]) const
    {
        BaseClass::save((int16_t*)data);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(uint16_t data[8]) const
    {
        BaseClass::saveAligned((int16_t*) data);
    }

    /** Stream aligned. Not safe to use until you exactly know what you are doing */
    void streamAligned(uint16_t data[8]) const
    {
        BaseClass::streamAligned((int16_t*) data);
    }


    /* Immediate shift operations */
    friend UInt16x8 operator >> (const UInt16x8 &left, uint32_t count);
    friend UInt16x8 operator >>= (UInt16x8 &left, uint32_t count);

    /* Shift operations */
    friend UInt16x8 operator >> (const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator >>= (UInt16x8 &left, const UInt16x8 &right);

    /* Comparison */
    friend UInt16x8 operator < (const UInt16x8 &left, const UInt16x8 &right);
    friend UInt16x8 operator > (const UInt16x8 &left, const UInt16x8 &right);

    /* Multiplication beware - overrun is possible*/
    friend UInt16x8 productHigherPart (const UInt16x8 &left, const UInt16x8 &right);

    /*Print to stream helper */
    friend ostream & operator << (ostream &out, const UInt16x8 &vector);
};



FORCE_INLINE UInt16x8 operator >> (const UInt16x8 &left, uint32_t count) {
    return UInt16x8(_mm_srli_epi16(left.data, count));
}

FORCE_INLINE UInt16x8 operator >>= (UInt16x8 &left, uint32_t count) {
    left.data = _mm_srli_epi16(left.data, count);
    return left;
}

FORCE_INLINE UInt16x8 operator >> (const UInt16x8 &left, const UInt16x8 &right) {
    return UInt16x8(_mm_srl_epi16(left.data, right.data));
}

FORCE_INLINE UInt16x8 operator >>= (UInt16x8 &left, const UInt16x8 &right) {
    left.data = _mm_srl_epi16(left.data, right.data);
    return left;
}

FORCE_INLINE UInt16x8 operator < (const UInt16x8 &left, const UInt16x8 &right) {
    return UInt16x8(_mm_cmplt_epi16(left.data, right.data));
}

FORCE_INLINE UInt16x8 operator > (const UInt16x8 &left, const UInt16x8 &right) {
    return UInt16x8(_mm_cmpgt_epi16(left.data, right.data));
}

FORCE_INLINE UInt16x8 productHigherPart (const UInt16x8 &left, const UInt16x8 &right) {
    return UInt16x8(_mm_mulhi_epu16(left.data, right.data));
}


} //namespace corecvs

#endif // UINT16X8_H
