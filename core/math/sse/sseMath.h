#pragma once
/**
 * \file sseMath.h
 * \brief Header with common interface for SSE and integres
 *
 * \ingroup cppcorefiles
 * \date Oct 24, 2010
 */

#include "global.h"

#include "sseWrapper.h"
#include "puzzleBlock.h"
#ifdef WITH_SSE4
#include <smmintrin.h>
#endif


namespace corecvs {


/**
 *   This class provides the base functions for SSE (and probably NEON)
 *   this creates common interface for SSE and ordinary integer types
 **/
class SSEMath
{
public:

    /* For signed types  */
    template <int divisor>
        static inline Int16x8 div(const Int16x8 &val);

    template <int multiplier>
        static inline Int16x8 mul(const Int16x8 &val);

    /* For unsigned types  */
    template <int divisor>
        static inline UInt16x8 div(const UInt16x8 &val);

    template <int multiplier>
        static inline UInt16x8 mul(const UInt16x8 &val);

    /* For double types */
    template <int multiplier>
        static inline Doublex2 mul(const Doublex2 &val);

    template <int divisor>
        static inline Doublex2 div(const Doublex2 &val);




    /* TODO: Unite the following lines */
    ALIGN_STACK_SSE static inline Int16x8 branchlessMask(const Int16x8 &val)
    {
        return val;
    }

    ALIGN_STACK_SSE static inline UInt16x8 branchlessMask(const UInt16x8 &val)
    {
        return val;
    }

    ALIGN_STACK_SSE static inline Int16x8 selector(const Int16x8 &condition, const Int16x8 &ifTrue, const Int16x8 &ifFalse)
    {
        return Int16x8((condition & ifTrue) | (andNot(condition, ifFalse)));
    }

    ALIGN_STACK_SSE static inline UInt16x8 selector(const UInt16x8 &condition, const UInt16x8 &ifTrue, const UInt16x8 &ifFalse)
    {
        return UInt16x8((condition & ifTrue) | (andNot(condition, ifFalse)));
    }

    ALIGN_STACK_SSE static inline Int32x4 selector(const Int32x4 &condition, const Int32x4 &ifTrue, const Int32x4 &ifFalse)
    {
        return (condition & ifTrue) | (andNot(condition, ifFalse));
    }

#ifdef WITH_AVX2
    ALIGN_STACK_SSE static inline Int32x8 selector(const Int32x8 &condition, const Int32x8 &ifTrue, const Int32x8 &ifFalse)
    {
        return (condition & ifTrue) | (andNot(condition, ifFalse));
    }
#endif

    ALIGN_STACK_SSE static inline void fillOnes(UInt16x8 &var)
    {
        var = UInt16x8::AllOnes();
    }

    /**
     * blockSAD8(): get UInt16x8 vector
     * \code
     *    [SAD(left,right) SAD(left+1,right) ... SAD(left+7,right) ]
     * \endcode
     * where, for example, left + 5 is
     *
     * \code
     *             left              left + 5
     *         +-----------+       +-----------+
     *         |_0_|_1_|_2_|_3_|_4_|_5_|___|___|
     *         |   |   |   |       |   |   |   |
     *         |___|___|___|   ->  |___|___|___|
     *         |   |   |   |       |   |   |   |
     *         +-----------+       +-----------+
     * \endcode
     *
     * and \f$SAD(left,right) = \sum_{i=0}^{N}|left(i)-right(i)|\f$
     **/
    static void blockSAD8(PuzzleBlock &left, PuzzleBlock &right, uint16_t* result)
    {
        UInt16x8 cost((uint16_t)0);

        uint16_t *dataL = NULL;
        uint16_t *dataR = NULL;

        for (int i = 0; i < left.h; i++)
        {
            dataL = left.data  + i * left.stride;
            dataR = right.data + i * right.stride;

            for (int j = 0; j < left.w; j++)
            {
                UInt16x8 valL(dataL);
                UInt16x8 valR(dataR[0]);
                UInt16x8 diff = difference(valL, valR);
                cost += diff;
                dataL++;
                dataR++;
            }
        }
        cost.save(result);
    }

    ALIGN_STACK_SSE inline static Int16x8 max (const Int16x8 &left, const Int16x8 &right) {
        return Int16x8(_mm_max_epi16(left.data, right.data));
    }

    ALIGN_STACK_SSE inline static Int16x8 min (const Int16x8 &left, const Int16x8 &right) {
        return Int16x8(_mm_min_epi16(left.data, right.data));
    }

    /* SSE4 has 4 comparison instructions for unsigned */
#ifdef WITH_SSE4
    ALIGN_STACK_SSE inline static UInt16x8 max (const UInt16x8 &left, const UInt16x8 &right) {
        return UInt16x8(_mm_max_epu16(left.data, right.data));
    }

    ALIGN_STACK_SSE inline static UInt16x8 min (const UInt16x8 &left, const UInt16x8 &right) {
        return UInt16x8(_mm_min_epu16(left.data, right.data));
    }
#endif

#ifdef WITH_AVX2
    ALIGN_STACK_SSE inline static Int16x16 max (const Int16x16 &left, const Int16x16 &right) {
        return Int16x16(_mm256_max_epi16(left.data, right.data));
    }

    ALIGN_STACK_SSE inline static Int16x16 min (const Int16x16 &left, const Int16x16 &right) {
        return Int16x16(_mm256_min_epi16(left.data, right.data));
    }
#endif


    ALIGN_STACK_SSE inline static UInt16x8 difference (const UInt16x8 &left, const UInt16x8 &right) {
#ifdef WITH_SSE4
        UInt16x8 min_tmp = min(left, right);
        UInt16x8 max_tmp = max(left, right);
        return UInt16x8(max_tmp - min_tmp);
#else
        return selector(left > right, (left - right), (right - left));
#endif
    }

    ALIGN_STACK_SSE inline static UInt16x8 difference15bit (const UInt16x8 &left, const UInt16x8 &right) {
    	Int16x8 leftSigned  = (Int16x8)left;
    	Int16x8 rightSigned = (Int16x8)right;

        Int16x8 min_tmp = min(leftSigned, rightSigned);
        Int16x8 max_tmp = max(leftSigned, rightSigned);
        return UInt16x8(max_tmp - min_tmp);
    }

    ALIGN_STACK_SSE inline static Int16x8 abs (const Int16x8 &value) {
    	return max(value, -value);
    }


};

/** Unsigned operations */
template<>
inline UInt16x8 SSEMath::div<2>(const UInt16x8 &val) {
    return val >> 1;
}

template<>
inline UInt16x8 SSEMath::div<4>(const UInt16x8 &val) {
    return val >> 2;
}

template<>
inline UInt16x8 SSEMath::div<16>(const UInt16x8 &val) {
    return val >> 4;
}

/* Signed operations*/
template<>
inline Int16x8 SSEMath::div<2>(const Int16x8 &val) {
    return val >> 1;    // shiftArithmetic(val, 1);
}

template<>
inline Int16x8 SSEMath::div<4>(const Int16x8 &val) {
    return val >> 2;    // shiftArithmetic(val, 2);
}

template<>
inline Int16x8 SSEMath::div<16>(const Int16x8 &val) {
    return val >> 4;    // shiftArithmetic(val, 4);
}


template<>
inline Int16x8 SSEMath::mul<5>(const Int16x8 &val) {
    return (val << 2) + val;
}

template<>
inline UInt16x8 SSEMath::mul<5>(const UInt16x8 &val) {
    return (val << 2) + val;
}



template<>
inline UInt16x8 SSEMath::div<5>(const UInt16x8 &val) {
    UInt16x8 a =  (val >> 1) + (val >> 2);
    a += (a >> 4) + (a >> 8) + (a >> 12);
    a >>= 2;
    UInt16x8 diff = val - mul<5>(a);
    a -= (diff > UInt16x8((uint16_t)4));
    return a;
}

template<>
inline UInt16x8 SSEMath::div<10>(const UInt16x8 &val) {
    return div<5>(div<2>(val));
}

template<>
inline Int16x8 SSEMath::mul<2>(const Int16x8 &val) {
    return val << 1;
}

template<>
inline Int16x8 SSEMath::mul<4>(const Int16x8 &val) {
    return val << 2;
}

template<>
inline Int16x8 SSEMath::mul<16>(const Int16x8 &val) {
    return val << 4;
}

template<>
inline Int16x8 SSEMath::mul<3>(const Int16x8 &val) {
    return (val << 1) + val;
}

template<>
inline UInt16x8 SSEMath::mul<2>(const UInt16x8 &val) {
    return val << 1;
}

/* For double types */
template<>
inline Doublex2 SSEMath::mul<2>(const Doublex2 &val) {
    return val + val;
}

template<>
inline Doublex2 SSEMath::mul<4>(const Doublex2 &val) {
    return val * Doublex2(4.0);
}

template<>
inline Doublex2 SSEMath::div<4>(const Doublex2 &val) {
    return val * Doublex2(0.25);
}


#if 0

ALIGN_STACK_SSE inline __m128i sse_div_u32_by_5(__m128i n) {
    __m128i q = _mm_add_epi32(_mm_srli_epi32(n,3),_mm_srli_epi32(n,4));
    q = _mm_add_epi32(q,_mm_srli_epi32(q,4));
    q = _mm_add_epi32(q,_mm_srli_epi32(q,8));
    q = _mm_add_epi32(q,_mm_srli_epi32(q,16));
    __m128i r = _mm_sub_epi32(n,_mm_add_epi32(_mm_slli_epi32(q,2),q));
    return _mm_add_epi32(q,_mm_srli_epi32(_mm_mullo_epi16(r,_mm_set1_epi16(13)),6));
}

ALIGN_STACK_SSE inline __m128i sse_div_u32_by_25(__m128i n) {
    __m128i q = _mm_add_epi32(_mm_srli_epi32(n,1),_mm_srli_epi32(n,3));
    q = _mm_add_epi32(q,_mm_srli_epi32(q,6));
    q = _mm_add_epi32(_mm_add_epi32(q,_mm_srli_epi32(q,7)),
                      _mm_add_epi32(_mm_srli_epi32(n,12),_mm_srli_epi32(n,15)));
    q = _mm_add_epi32(q,_mm_srli_epi32(q,20));
    q = _mm_srli_epi32(q,4);
    __m128i r = _mm_sub_epi32(_mm_sub_epi32(n,_mm_slli_epi32(q,4)),_mm_add_epi32(q,_mm_slli_epi32(q,3))); // q*25
    return _mm_sub_epi32(q,_mm_cmpgt_epi32(r,_mm_set1_epi32(24))); // r>=25 -> q - (-1)
}

ALIGN_STACK_SSE inline __m128i sse_div_u32_by_25_fast(__m128i n) {
    __m128i inverse = _mm_set1_epi16   (0xA3D7);
    __m128i shifted = _mm_srli_epi32  (n, 1);
    /* Now high 16 bits of n are empty*/
    __m128i mulhi   = _mm_mulhi_epu16(inverse, shifted);
    __m128i q       = _mm_srli_epi32  (mulhi,3);

    // r = (n - (q * 16)) - (q + q * 8) = n - 25 * q;
    __m128i r = _mm_sub_epi32(_mm_sub_epi32(n,_mm_slli_epi32(q,4)),_mm_add_epi32(q,_mm_slli_epi32(q,3))); // q*25
    return _mm_sub_epi32(q,_mm_cmpgt_epi32(r,_mm_set1_epi32(24))); // r>=25 -> q - (-1)
}

ALIGN_STACK_SSE inline __m128i sse_div_u32_by_25_fp(__m128i n) {
    return _mm_cvttps_epi32(_mm_mul_ps(_mm_cvtepi32_ps(n),_mm_set1_ps(1.0/25)));
}


#endif

} //namespace corecvs

/* EOF */

