#ifndef INTBASE16X8_H
#define INTBASE16X8_H
/**
 * \file intBase16x8.h
 * \brief
 *
 * \ingroup cppcorefiles
 * \date Aug 31, 2012
 * \author: a.melnikov
 */

#include <emmintrin.h>
#include <stdint.h>

#include "core/utils/global.h"
#include "core/math/vector/fixedVector.h"
#include "core/math/sse/sseInteger.h"
#include "core/math/sse/int32x4.h"
#include "core/math/sse/int32x8v.h"

namespace corecvs {

/**
 *  This class is a wrapper around data type that stores 8 16bit integers in 128 register
 *
 **/
template<typename RealType>
class ALIGN_DATA(16) IntBase16x8 : public SSEInteger<RealType>
{
public:
    typedef SSEInteger<RealType> SSEBase;

    /* Constructors */

    IntBase16x8() {}

    /**
     *  Copy constructor
     **/
    IntBase16x8(const IntBase16x8 &other) : SSEBase(other) {}

    template<class OtherChild>
    explicit IntBase16x8(const SSEInteger<OtherChild> &other) : SSEBase(other.data) {}


    /**
     *  Create SSE integer vector from integer constant
     **/
    IntBase16x8(const __m128i &_data) :
            SSEBase(_data) {}

    explicit IntBase16x8(int16_t constant) :
            SSEBase(_mm_set1_epi16(constant)) {}

    explicit IntBase16x8(uint16_t constant) :
            SSEBase(_mm_set1_epi16(constant)) {}

    explicit IntBase16x8(int16_t c0, int16_t c1, int16_t c2, int16_t c3,
                         int16_t c4, int16_t c5, int16_t c6, int16_t c7) :
            SSEBase(_mm_set_epi16(c7, c6, c5, c4, c3, c2, c1, c0)) {}

    explicit IntBase16x8(const int16_t  * const data_ptr) :
            SSEBase((__m128i *)data_ptr) {}

    explicit IntBase16x8(const uint16_t * const data_ptr) :
            SSEBase((__m128i *)data_ptr) {}

    explicit inline IntBase16x8(const FixedVector<int16_t,8> input) :
            SSEBase((__m128i *)&input.element[0]) {}

#if DEPRECATED_CONTR
    explicit inline IntBase16x8(const Int32x4 &value) :
            SSEBase(value.data) {}
#endif


    explicit inline IntBase16x8(const Int32x8v &value) :
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
    static RealType pack(const Int32x4 &first, const Int32x4 &second)
    {
        return RealType(_mm_packs_epi32(first.data, second.data));
    }

    /** Load unaligned. */
    static RealType load(const int16_t data[8])
    {
        return RealType(SSEBase::load((const __m128i *)data).data);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static RealType loadAligned(const int16_t data[8])
    {
        return RealType(SSEBase::loadAligned((const __m128i *)data).data);
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

    //inline Int32x8v expand() const

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
    inline RealType shuffled() const
    {
        return RealType(_mm_shuffle_epi32(this->data, imm));
    }

    /* Arithmetics operations */

    friend FORCE_INLINE RealType operator + (const RealType &left, const RealType &right) {
        return RealType(_mm_add_epi16(left.data, right.data));
    }

    friend FORCE_INLINE RealType operator - (const RealType &left, const RealType &right) {
        return RealType(_mm_sub_epi16(left.data, right.data));
    }

    friend FORCE_INLINE RealType operator += (RealType &left, const RealType &right) {
        left.data = _mm_add_epi16(left.data, right.data);
        return left;
    }

    friend FORCE_INLINE RealType operator -= (RealType &left, const RealType &right) {
        left.data = _mm_sub_epi16(left.data, right.data);
        return left;
    }

    /* Immediate shift operations */
    friend FORCE_INLINE RealType operator << (const RealType &left, uint32_t count) {
        return RealType(_mm_slli_epi16(left.data, count));
    }

    friend FORCE_INLINE RealType operator <<= (RealType &left, uint32_t count) {
        left.data = _mm_slli_epi16(left.data, count);
        return left;
    }

    /* Shift operations */
    friend FORCE_INLINE RealType operator << (const RealType &left, const RealType &right) {
        return RealType(_mm_sll_epi16(left.data, right.data));
    }

    friend FORCE_INLINE RealType operator <<= (RealType &left, const RealType &right) {
        left.data = _mm_sll_epi16(left.data, right.data);
        return left;
    }

    /* Comparison */
    friend FORCE_INLINE RealType operator == (const RealType &left, const RealType &right) {
        return RealType(_mm_cmpeq_epi16(left.data, right.data));
    }

    /* Multiplication beware - overrun is possible*/
    friend FORCE_INLINE RealType productLowerPart (const RealType &left, const RealType &right) {
        return RealType(_mm_mullo_epi16(left.data, right.data));
    }

    friend FORCE_INLINE RealType operator * (const RealType &left, const RealType &right) {
        return productLowerPart(left, right);
    }

    friend FORCE_INLINE RealType operator *= (RealType &left, const RealType &right) {
        left = productLowerPart(left, right);
        return left;
    }

    /* Slow but helpful */
    friend FORCE_INLINE RealType operator * (int16_t left, RealType &right) {
        return right * RealType(left);
    }

    friend FORCE_INLINE RealType operator * (const RealType &left, int16_t right) {
        return left * RealType(right);
    }

    friend FORCE_INLINE RealType operator *= (RealType &left, int16_t right) {
        left = left * right;
        return left;
    }

};


} //namespace corecvs

#endif // INTBASE16X8_H
