#ifndef _SSEBINARY_H_
#define _SSEBINARY_H_
/**
 * \file sseInteger.h
 * \brief This class holds the SSEInteger class
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

/**
 *   This template class holds the base class for integer sse types.
 *
 *   There are operations that don't work with 128 as separate integers but no
 *   more then the bits. This class holds logical bit operations.
 *
 *
 **/
template<typename RealType>
class ALIGN_DATA(16) SSEInteger
{
public:

    __m128i data;
    /* Constructors */
    SSEInteger() {}

    explicit SSEInteger(const __m128i * const data_ptr)
    {
        this->data = _mm_loadu_si128(data_ptr);
    }

    explicit SSEInteger(const __m128i &data)
    {
        this->data = data;
    }

    /** Load unaligned. Not safe to use until you exactly know what you are doing */
    static SSEInteger load(const __m128i * const data_ptr)
    {
        return SSEInteger(data_ptr);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static SSEInteger loadAligned(const __m128i * const data_ptr)
    {
        return SSEInteger(_mm_load_si128(data_ptr));
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
#endif // PROFILE_ACCESS_ALIGNMENT

    void save(__m128i * const data) const
    {
        _mm_storeu_si128(data, this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        if (((uint64_t)(void *)data) % 16)
            unalignedUWrites++;
        else
            alignedUWrites++;
#endif
    }

    /** Save aligned.
     * \remark Not safe to use until you exactly know what you are doing
     **/
    void saveAligned(__m128i * const data) const
    {
        _mm_store_si128(data, this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
         alignedAWrites++;
#endif
    }

    /**
     * Stream aligned.
     * \remark Not safe to use until you exactly know what you are doing
     * */
    void streamAligned(__m128i * const data) const
    {
        _mm_stream_si128((__m128i *)data, this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        streamedWrites++;
#endif
    }


    /* Logical operations */
    friend RealType operator & (const RealType &left, const SSEInteger &right) {
        return RealType(_mm_and_si128(left.data, right.data));
    }

    friend RealType operator | (const RealType &left, const SSEInteger &right) {
        return RealType(_mm_or_si128(left.data, right.data));
    }

    friend RealType operator ^ (const RealType &left, const SSEInteger &right) {
        return RealType(_mm_xor_si128(left.data, right.data));
    }

    friend RealType andNot (const RealType &left, const SSEInteger &right) {
        return RealType(_mm_andnot_si128(left.data, right.data));
    }

    friend RealType operator &= (RealType &left, const SSEInteger &right) {
        left.data = _mm_and_si128(left.data, right.data);
        return left;
    }

    friend RealType operator |= (RealType &left, const SSEInteger &right) {
        left.data = _mm_or_si128(left.data, right.data);
        return left;
    }

    friend RealType operator ^= (RealType &left, const SSEInteger &right) {
        left.data = _mm_xor_si128(left.data, right.data);
        return left;
    }

    friend RealType andNotThis (RealType &left, const SSEInteger &right) {
        left.data = _mm_andnot_si128(left.data, right.data);
        return left;
    }

};


} //namespace corecvs

#endif //_SSEBINARY_H_
