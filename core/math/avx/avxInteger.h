#ifndef _AVX_INTEGER_H_
#define _AVX_INTEGER_H_
/**
 * \file avxInteger.h
 * \brief This class holds the AVXInteger class
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include <emmintrin.h>
#include <immintrin.h>
#include <stdint.h>

#include "core/utils/global.h"
#include "core/math/vector/fixedVector.h"

namespace corecvs {

/**
 *   This template class holds the base class for integer sse types.
 *
 *   There are operations that don't work with 256 as separate integers but no
 *   more then the bits. This class holds logical bit operations.
 *
 *
 **/
template<typename RealType>
class ALIGN_DATA(32) AVXInteger
{
public:

    __m256i data;
    /* Constructors */
    AVXInteger() {}

    explicit AVXInteger(const __m256i * const data_ptr)
    {
        this->data = _mm256_loadu_si256(data_ptr);
    }

    explicit AVXInteger(const __m256i &data)
    {
     //   printf("AVXInteger(const __m128i &data):Setting data\n");
        this->data = data;
    }

    static RealType Zero() {
        return RealType(_mm256_setzero_si256());
    }

    static RealType AllOnes() {
        return RealType(_mm256_set1_epi32(0xFFFF));
    }

    /** Load unaligned. Not safe to use until you exactly know what you are doing */
    static AVXInteger load(const __m256i * const data_ptr)
    {
        return AVXInteger(data_ptr);
    }

    /** Load aligned. Not safe to use until you exactly know what you are doing */
    static AVXInteger loadAligned(const __m256i * const data_ptr)
    {
        return AVXInteger(_mm256_load_si256(data_ptr));
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

    void save(__m256i * const data) const
    {
        _mm256_storeu_si256(data, this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        if (((uint64_t)(void *)data) % 16)
            unalignedUWrites++;
        else
            alignedUWrites++;
#endif
    }

    /**
     * Save aligned.
     * \remark Not safe to use until you exactly know what you are doing
     **/
    void saveAligned(__m256i * const data) const
    {
        _mm256_store_si256(data, this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
         alignedAWrites++;
#endif
    }

    /**
     * Stream aligned.
     * \remark Not safe to use until you exactly know what you are doing
     * */
    void streamAligned(__m256i * const data) const
    {
        _mm256_stream_si256((__m256i *)data, this->data);
#ifdef PROFILE_ACCESS_ALIGNMENT
        streamedWrites++;
#endif
    }


    /* Logical operations */
    friend RealType operator & (const RealType &left, const AVXInteger &right) {
     //   printf("called operator &\n");
        return RealType(_mm256_and_si256(left.data, right.data));
    }

    friend RealType operator | (const RealType &left, const AVXInteger &right) {
        return RealType(_mm256_or_si256(left.data, right.data));
    }

    friend RealType operator ^ (const RealType &left, const AVXInteger &right) {
        return RealType(_mm256_xor_si256(left.data, right.data));
    }

    friend RealType andNot (const RealType &left, const AVXInteger &right) {
        return RealType(_mm256_andnot_si256(left.data, right.data));
    }

    friend RealType operator &= (RealType &left, const AVXInteger &right) {
        left.data = _mm256_and_si256(left.data, right.data);
        return left;
    }

    friend RealType operator |= (RealType &left, const AVXInteger &right) {
        left.data = _mm256_or_si256(left.data, right.data);
        return left;
    }

    friend RealType operator ^= (RealType &left, const AVXInteger &right) {
        left.data = _mm256_xor_si256(left.data, right.data);
        return left;
    }

    friend RealType andNotThis (RealType &left, const AVXInteger &right) {
        left.data = _mm256_andnot_si256(left.data, right.data);
        return left;
    }

    template <int shift>
    RealType shiftRightWhole() const
    {
        return RealType(_mm256_srli_si256(data, shift));       // for msvc2013 shift must be a constant
    }

};


} //namespace corecvs

#endif //_AVX_INTEGER_H_
