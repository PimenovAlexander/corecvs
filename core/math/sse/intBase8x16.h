#ifndef INTBASE8x16_H_
#define INTBASE8x16_H_
/**
 * \file IntBase8x16.h
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

template<typename RealType>
class ALIGN_DATA(16) IntBase8x16 : public SSEInteger<RealType>
{
public:
    typedef SSEInteger<RealType> SSEBase;


    IntBase8x16() {}

    /**
     *  Copy constructor
     **/
    IntBase8x16(const IntBase8x16 &other) : SSEBase(other) {}

    explicit IntBase8x16(const SSEBase &other) : SSEBase(other) {}

    /**
     *  Create SSE integer vector from integer constant
     **/

    IntBase8x16(const __m128i &_data) :
            SSEBase(_data) {}

    explicit IntBase8x16(int8_t constant) :
            SSEBase(_mm_set1_epi8(constant)){}

    explicit IntBase8x16(uint8_t constant) :
            SSEBase(_mm_set1_epi8(constant)) {}

    explicit IntBase8x16( int8_t  c0, int8_t  c1, int8_t  c2, int8_t  c3,
                      int8_t  c4, int8_t  c5, int8_t  c6, int8_t  c7,
                      int8_t  c8, int8_t  c9, int8_t c10, int8_t c11,
                      int8_t c12, int8_t c13, int8_t c14, int8_t c15 ) :
             SSEBase ( _mm_set_epi8(
                c15, c14, c13, c12, c11, c10, c9, c8,
                c7,  c6,  c5,  c4,  c3,  c2,  c1, c0 )) {}

    explicit IntBase8x16(const int8_t data[16]) {
        this->data = _mm_loadu_si128((__m128i *)&data[0]);
    }

    explicit IntBase8x16(const uint8_t data[16]) {
        this->data = _mm_loadu_si128((__m128i *)&data[0]);
    }

    explicit inline IntBase8x16(const FixedVector<int8_t,16> input)  {
        this->data = _mm_loadu_si128((__m128i *)&input.element[0]);
    }

#if DEPRECATED_CONTR
    explicit inline IntBase8x16(const Int32x4 &value)
    {
        this->data = value.data;
    }

    explicit inline IntBase8x16(const Int16x8 &value)
    {
        this->data = value.data;
    }
#endif

    explicit inline IntBase8x16(const Int32x8 &value)
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
    static IntBase8x16 pack(const Int16x8 &first, const Int16x8 &second)
    {
        return IntBase8x16(_mm_packs_epi16(first.data, second.data));
    }

    static IntBase8x16 packUnsigned(const Int16x8 &first, const Int16x8 &second)
    {
        return IntBase8x16(_mm_packus_epi16(first.data, second.data));
    }

    /** Load unaligned. Not safe to use until you exactly know what you are doing */
    static IntBase8x16 load(const int8_t *data)
    {
        return IntBase8x16(data);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static IntBase8x16 loadAligned(const int8_t data[16])
    {
        return IntBase8x16(_mm_load_si128((__m128i *)&data[0]));
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
    inline Int32x8 expand() const
    {
        return Int32x8(
                unpackLower4 (*this, IntBase8x16((uint16_t)0)),
                unpackHigher4(*this, IntBase8x16((uint16_t)0))
                );
    }
#endif

    /* Arithmetics operations */
    friend FORCE_INLINE RealType operator + (const RealType &left, const RealType &right) {
        return RealType(_mm_add_epi8(left.data, right.data));
    }

    friend FORCE_INLINE RealType operator - (const RealType &left, const RealType &right) {
        return RealType(_mm_sub_epi8(left.data, right.data));
    }

    friend FORCE_INLINE RealType operator += (RealType &left, const RealType &right) {
        left.data = _mm_add_epi8(left.data, right.data);
        return left;
    }

    friend FORCE_INLINE RealType operator -= (RealType &left, const RealType &right) {
        left.data = _mm_sub_epi8(left.data, right.data);
        return left;
    }

    IntBase8x16 operator -( ) {
        return (IntBase8x16((int8_t)0) - *this);
    }

    /* Immediate shift operations */
#ifdef UNSUPPORTED
    friend FORCE_INLINE RealType operator << (const RealType &left, uint32_t count) {
        return RealType(_mm_slli_epi16(left.data, count));  // _mm_slli_epi8 is absent!
    }

    friend FORCE_INLINE RealType operator >> (const RealType &left, uint32_t count) {
        return RealType(_mm_srai_epi16(left.data, count));  // _mm_srai_epi8 is absent!
    }

    friend FORCE_INLINE RealType shiftLogical(const RealType &left, uint32_t count) {
        return RealType(_mm_srli_epi16(left.data, count));  // _mm_srli_epi8 is absent!
    }

    friend FORCE_INLINE RealType operator <<= (RealType &left, uint32_t count) {
        left.data = _mm_slli_epi16(left.data, count);  // _mm_slli_epi8 is absent!
        return left;
    }

    friend FORCE_INLINE RealType operator >>= (RealType &left, uint32_t count) {
        left.data = _mm_srai_epi16(left.data, count);  // _mm_srai_epi8 is absent!
        return left;
    }

    friend FORCE_INLINE RealType operator << (const RealType &left, const RealType &right) {
        return RealType(_mm_sll_epi16(left.data, right.data));  // _mm_sll_epi8 is absent!
    }

    friend FORCE_INLINE RealType operator >> (const RealType &left, const RealType &right) {
        return RealType(_mm_sra_epi16(left.data, right.data));  // _mm_sra_epi8 is absent!
    }

    friend FORCE_INLINE RealType shiftLogical(const RealType &left, const RealType &right) {
        return RealType(_mm_srl_epi16(left.data, right.data));  // _mm_srl_epi8 is absent!
    }

    friend FORCE_INLINE RealType operator <<= (RealType &left, const RealType &right) {
        left.data = _mm_sll_epi16(left.data, right.data);  // _mm_sll_epi8 is absent!
        return left;
    }

    friend FORCE_INLINE RealType operator >>= (RealType &left, const RealType &right) {
        left.data = _mm_sra_epi16(left.data, right.data);  // _mm_srl_epi8 is absent!
        return left;
    }
#endif

    /* Comparison */
    friend FORCE_INLINE RealType operator == (const RealType &left, const RealType &right) {
        return RealType(_mm_cmpeq_epi8(left.data, right.data));
    }

#ifdef UNSUPPORTED
    friend IntBase8x16 productLowerPart (const IntBase8x16 &left, const IntBase8x16 &right);
    friend IntBase8x16 productHigherPart(const IntBase8x16 &left, const IntBase8x16 &right);
    friend Int32x8 productExtending (const IntBase8x16 &left, const IntBase8x16 &right);

    /* Multiplication beware - overrun is possible*/
    friend FORCE_INLINE IntBase8x16 operator * (const IntBase8x16 &left, const IntBase8x16 &right) {
        return productLowerPart(left, right);
    }

    friend FORCE_INLINE IntBase8x16 operator *= (IntBase8x16 &left, const IntBase8x16 &right) {
        left = productLowerPart(left, right);
        return left;
    }

    friend FORCE_INLINE IntBase8x16 operator * (int8_t left, IntBase8x16 &right) {
        return right * IntBase8x16(left);
    }

    friend FORCE_INLINE IntBase8x16 operator * (const IntBase8x16 &left, int8_t right) {
        return left * IntBase8x16(right);
    }

    friend FORCE_INLINE IntBase8x16 operator *= (IntBase8x16 &left, int8_t right) {
        left = left * right;
        return left;
    }

#endif

    /*Print to stream helper */

//    friend ostream & operator << (ostream &out, const IntBase8x16 &vector);

    /*TODO: Add shuffling*/

    /**
     *  This function unpacks two 8bit vectors into 16bit vector using 0-7 elements
     *
     *
     *  left  ->   A0 A1 A2 A3 .... A15 A16
     *  right ->   B0 B1 B2 B3 .... B15 B16
     *
     *  result ->  A0 B0 A1 B2 .... A7  B7
     *
     **/
    inline static Int16x8 unpackLower (const IntBase8x16 &left, const IntBase8x16 &right) {
       return Int16x8(_mm_unpacklo_epi8(left.data, right.data));
    }

    inline static Int16x8 unpackHigher (const IntBase8x16 &left, const IntBase8x16 &right) {
       return Int16x8(_mm_unpackhi_epi8(left.data, right.data));
    }


    FORCE_INLINE IntBase8x16 productLowerPart (const IntBase8x16 &left, const IntBase8x16 &right) {
        return IntBase8x16(_mm_mullo_epi16(left.data, right.data));
    }

    FORCE_INLINE IntBase8x16 productHigherPart (const IntBase8x16 &left, const IntBase8x16 &right) {
        return IntBase8x16(_mm_mulhi_epi16(left.data, right.data));
    }



};

} //namespace corecvs

#endif //#ifndef INTBASE8x16_H_
