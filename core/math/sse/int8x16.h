#ifndef INT8X16_H_
#define INT8X16_H_
/**
 * \file int8x16.h
 * \brief
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include <emmintrin.h>
#include <stdint.h>

#include "core/utils/global.h"
#include "core/math/vector/fixedVector.h"
#include "core/math/sse/intBase8x16.h"

namespace corecvs {


class ALIGN_DATA(16) Int8x16 : public IntBase8x16<Int8x16>
{
public:
    typedef IntBase8x16<Int8x16> BaseClass;

    /**
     * Constructors
     **/
    Int8x16() {}

    /**
     *  Copy constructor
     **/
    Int8x16(const Int8x16 &other) : BaseClass(other) {}

    template<class Sibling>
    explicit Int8x16(const IntBase8x16<Sibling> &other)  : BaseClass(other) {}

    template<class Sibling>
    explicit Int8x16(IntBase8x16<Sibling> &other) : BaseClass(other) {}

    explicit Int8x16(const SSEBase &other) : BaseClass(other) {}

    /**
     *  Create SSE integer vector from integer constant
     **/

    Int8x16(const __m128i &_data) :
        BaseClass(_data) {}

    explicit Int8x16(int8_t constant) :
        BaseClass(constant){}

    explicit Int8x16(uint8_t constant) :
        BaseClass(constant) {}

    explicit Int8x16( int8_t  c0, int8_t  c1, int8_t  c2, int8_t  c3,
                      int8_t  c4, int8_t  c5, int8_t  c6, int8_t  c7,
                      int8_t  c8, int8_t  c9, int8_t c10, int8_t c11,
                      int8_t c12, int8_t c13, int8_t c14, int8_t c15 ) :
        BaseClass (c0, c1,  c2,  c3,  c4,  c5,  c6,  c7,
                   c8, c9, c10, c11, c12, c13, c14, c15) {}

    explicit Int8x16(const int8_t data[16]) :
        BaseClass(data) {}

    explicit Int8x16(const uint8_t data[16]) :
        BaseClass(data) {}

    explicit inline Int8x16(const FixedVector<int8_t,16> input) :
        BaseClass(input) {}

/* Could we use template constructor here? */
    explicit inline Int8x16(const Int32x4 &value)
    {
        this->data = value.data;
    }

    explicit inline Int8x16(const Int16x8 &value)
    {
        this->data = value.data;
    }

    explicit inline Int8x16(const Int32x8v &value)
    {
        this->data =  _mm_packs_epi32(value.element[0].data, value.element[1].data);
    }

    /* Static fabrics */

    /**
     *   This functions packs the lower parts of each of elements of input registers into one.
     *   Putting one after the other.
     *
     *   <pre>
     *    bits      0            16           32
     *   first  :   | A0.0  A0.1 | A1.0  A1.1 | A2.0  A2.1 ...
     *   second :   | B0.0  B0.1 | B1.0  B1.1 | B2.0  B2.1 ...
     *
     *   result :   | A0.0  A1.0 | A2.0  ...  | A6.0 A7.0 | B0.0  B1.0 |  ...  | B6.0 B7.0
     *   </pre>
     *
     **/
    static Int8x16 pack(const Int16x8 &first, const Int16x8 &second)
    {
        return Int8x16(_mm_packs_epi16(first.data, second.data));
    }

    static Int8x16 packUnsigned(const Int16x8 &first, const Int16x8 &second)
    {
        return Int8x16(_mm_packus_epi16(first.data, second.data));
    }


    /** Load unaligned. Not safe to use until you exactly know what you are doing */
    static Int8x16 load(const int8_t *data)
    {
        return Int8x16(data);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static Int8x16 loadAligned(const int8_t data[16])
    {
        return Int8x16(_mm_load_si128((__m128i *)&data[0]));
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

    void save(int8_t data[16]) const
    {
        _mm_storeu_si128((__m128i *)&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        if (((uint64_t)(void *)&data[0]) % 16)
            unalignedUWrites++;
        else
            alignedUWrites++;
#endif
    }

    void save(uint8_t data[16]) const
    {
        save((int8_t *)&data[0]);
    }

    /** Save aligned. Not safe to use until you exactly know what you are doing */
    void saveAligned(int8_t data[16]) const
    {
        _mm_store_si128((__m128i *)&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
         alignedAWrites++;
#endif
    }

    void saveAligned(uint8_t data[16]) const
    {
        save((int8_t *)&data[0]);
    }

    /** Stream aligned. Not safe to use until you exactly know what you are doing */
    void streamAligned(int8_t data[16]) const
    {
        _mm_stream_si128((__m128i *)&data[0], this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        streamedWrites++;
#endif
    }

    /* converters */
template<int idx>
    uint8_t getInt() const
    {
        /*TODO: SSE4 actually has _mm_extract_epi8*/
        uint16_t tmp = _mm_extract_epi16(this->data, idx >> 1);
        if (idx & 0x1)
            return (tmp >> 8) & 0xFF;
        else
            return tmp & 0xFF;
    }

    inline uint16_t operator[] (uint32_t idx) const
    {
        switch (idx) {
            case  0: return getInt<0>();
            case  1: return getInt<1>();
            case  2: return getInt<2>();
            case  3: return getInt<3>();
            case  4: return getInt<4>();
            case  5: return getInt<5>();
            case  6: return getInt<6>();
            case  7: return getInt<7>();
            case  8: return getInt<8>();
            case  9: return getInt<9>();
            case 10: return getInt<10>();
            case 11: return getInt<11>();
            case 12: return getInt<12>();
            case 13: return getInt<13>();
            case 14: return getInt<14>();
            case 15: return getInt<15>();

        }
        return 0;
    }

    inline uint16_t maskToInt() const
    {
        return _mm_movemask_epi8(this->data);
    }

#ifdef NYI
    inline Int32x8v expand() const
    {
        return Int32x8(
                unpackLower4 (*this, Int8x16((uint16_t)0)),
                unpackHigher4(*this, Int8x16((uint16_t)0))
                );
    }
#endif

    /* Saturated arithmetics operations */
    friend Int8x16 adds(const Int8x16 &left, const Int8x16 &right);
    friend Int8x16 subs(const Int8x16 &left, const Int8x16 &right);

    Int8x16 operator -( ) {
        return (Int8x16((int8_t)0) - *this);
    }

    /* Immediate shift operations */
    friend Int8x16 operator << (const Int8x16 &left, uint32_t count);
    friend Int8x16 operator >> (const Int8x16 &left, uint32_t count);
    friend Int8x16 shiftLogical(const Int8x16 &left, uint32_t count);

    friend Int8x16 operator <<= (Int8x16 &left, uint32_t count);
    friend Int8x16 operator >>= (Int8x16 &left, uint32_t count);

    /* Shift operations */
    friend Int8x16 operator << (const Int8x16 &left, const Int8x16 &right);
    friend Int8x16 operator >> (const Int8x16 &left, const Int8x16 &right);
    friend Int8x16 shiftLogical(const Int8x16 &left, const Int8x16 &right);

    friend Int8x16 operator <<= (Int8x16 &left, const Int8x16 &right);
    friend Int8x16 operator >>= (Int8x16 &left, const Int8x16 &right);

    /* Comparison */
    friend Int8x16 operator <  (const Int8x16 &left, const Int8x16 &right);
    friend Int8x16 operator >  (const Int8x16 &left, const Int8x16 &right);
    friend Int8x16 operator == (const Int8x16 &left, const Int8x16 &right);

    /* Multiplication beware - overrun is possible*/
    friend Int8x16 operator *  (const Int8x16 &left, const Int8x16 &right);
    friend Int8x16 operator *= (const Int8x16 &left, const Int8x16 &right);

    friend Int8x16 productLowerPart (const Int8x16 &left, const Int8x16 &right);
    friend Int8x16 productHigherPart(const Int8x16 &left, const Int8x16 &right);
    friend Int32x8v productExtending (const Int8x16 &left, const Int8x16 &right);

    /* Slow but helpful */
    friend Int8x16 operator *  (      int16_t right, Int8x16 &left);
    friend Int8x16 operator *  (const Int8x16 &left, int16_t right);
    friend Int8x16 operator *= (      Int8x16 &left, int16_t right);


    inline static Int16x8 unpackLower (const Int8x16 &left, const Int8x16 &right) {
        return Int16x8(_mm_unpacklo_epi8(left.data, right.data));
    }

    inline static Int16x8 unpackHigher (const Int8x16 &left, const Int8x16 &right) {
        return Int16x8(_mm_unpackhi_epi8(left.data, right.data));
    }

    /* Print to stream helper */

    friend ostream & operator << (ostream &out, const Int8x16 &vector);

    /* Formated hex helper */
    void hexDump (ostream &out);
};

#ifdef UNSUPPORTED
FORCE_INLINE Int8x16 operator << (const Int8x16 &left, uint32_t count) {
    return Int8x16(_mm_slli_epi16(left.data, count));  // _mm_slli_epi8 is absent!
}

FORCE_INLINE Int8x16 operator >> (const Int8x16 &left, uint32_t count) {
    return Int8x16(_mm_srai_epi16(left.data, count));  // _mm_srai_epi8 is absent!
}

FORCE_INLINE Int8x16 shiftLogical(const Int8x16 &left, uint32_t count) {
    return Int8x16(_mm_srli_epi16(left.data, count));  // _mm_srli_epi8 is absent!
}

FORCE_INLINE Int8x16 operator <<= (Int8x16 &left, uint32_t count) {
    left.data = _mm_slli_epi16(left.data, count);  // _mm_slli_epi8 is absent!
    return left;
}

FORCE_INLINE Int8x16 operator >>= (Int8x16 &left, uint32_t count) {
    left.data = _mm_srai_epi16(left.data, count);  // _mm_srai_epi8 is absent!
    return left;
}

FORCE_INLINE Int8x16 operator << (const Int8x16 &left, const Int8x16 &right) {
    return Int8x16(_mm_sll_epi16(left.data, right.data));  // _mm_sll_epi8 is absent!
}

FORCE_INLINE Int8x16 operator >> (const Int8x16 &left, const Int8x16 &right) {
    return Int8x16(_mm_sra_epi16(left.data, right.data));  // _mm_sra_epi8 is absent!
}

FORCE_INLINE Int8x16 shiftLogical(const Int8x16 &left, const Int8x16 &right) {
    return Int8x16(_mm_srl_epi16(left.data, right.data));  // _mm_srl_epi8 is absent!
}

FORCE_INLINE Int8x16 operator <<= (Int8x16 &left, const Int8x16 &right) {
    left.data = _mm_sll_epi16(left.data, right.data);  // _mm_sll_epi8 is absent!
    return left;
}

FORCE_INLINE Int8x16 operator >>= (Int8x16 &left, const Int8x16 &right) {
    left.data = _mm_sra_epi16(left.data, right.data);  // _mm_srl_epi8 is absent!
    return left;
}
#endif

FORCE_INLINE Int8x16 adds(const Int8x16 &left, const Int8x16 &right) {
    return Int8x16(_mm_adds_epi8(left.data, right.data));
}

FORCE_INLINE Int8x16 subs(const Int8x16 &left, const Int8x16 &right) {
    return Int8x16(_mm_subs_epi8(left.data, right.data));
}



FORCE_INLINE Int8x16 operator < (const Int8x16 &left, const Int8x16 &right) {
    return Int8x16(_mm_cmplt_epi8(left.data, right.data));
}

FORCE_INLINE Int8x16 operator > (const Int8x16 &left, const Int8x16 &right) {
    return Int8x16(_mm_cmpgt_epi8(left.data, right.data));
}

FORCE_INLINE Int8x16 productLowerPart (const Int8x16 &left, const Int8x16 &right) {
    return Int8x16(_mm_mullo_epi16(left.data, right.data));
}

FORCE_INLINE Int8x16 productHigherPart (const Int8x16 &left, const Int8x16 &right) {
    return Int8x16(_mm_mulhi_epi16(left.data, right.data));
}

FORCE_INLINE Int8x16 operator * (const Int8x16 &left, const Int8x16 &right) {
    return productLowerPart(left, right);
}

FORCE_INLINE Int8x16 operator *= (Int8x16 &left, const Int8x16 &right) {
    left = productLowerPart(left, right);
    return left;
}

/*
FORCE_INLINE Int16x16 productExtending (const Int8x16 &left, const Int8x16 &right)
{
    Int8x16  lowParts = productLowerPart (left, right);
    Int8x16 highParts = productHigherPart(left, right);

    return Int16x16(unpackLower4(lowParts, highParts), unpackHigher4(lowParts, highParts));
}
*/

FORCE_INLINE Int8x16 operator * (int8_t left, Int8x16 &right) {
    return right * Int8x16(left);
}

FORCE_INLINE Int8x16 operator * (const Int8x16 &left, int8_t right) {
    return left * Int8x16(right);
}

FORCE_INLINE Int8x16 operator *= (Int8x16 &left, int8_t right) {
    left = left * right;
    return left;
}


} //namespace corecvs

#endif //#ifndef INT8X16_H_
