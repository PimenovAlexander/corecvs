#ifndef NEON_MATH_H_
#define NEON_MATH_H_

/**
 * \file sseMath.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Oct 24, 2010
 * \author alexander
 */

#include "core/utils/global.h"
namespace corecvs {

class SSEMath
{
public:
    template <int divisor>
        static inline Int16x8 div(const Int16x8 &val);

    template <int multiplier>
        static inline Int16x8 mul(const Int16x8 &val);


    template <int divisor>
        static inline UInt16x8 div(const UInt16x8 &val);

    template <int multiplier>
        static inline UInt16x8 mul(const UInt16x8 &val);



    template <int divisor>
        static inline Int16x8 divU(const Int16x8 &val);

    ALIGN_STACK_SSE static inline Int16x8 branchlessMask(const Int16x8 &val)
    {
        return val;
    }

    ALIGN_STACK_SSE static inline Int16x8 selector(const Int16x8 &condition, const Int16x8 &ifTrue, const Int16x8 &ifFalse)
    {
        return (condition & ifTrue) | (andNot(condition, ifFalse));
    }

    ALIGN_STACK_SSE static inline Int32x4 selector(const Int32x4 &condition, const Int32x4 &ifTrue, const Int32x4 &ifFalse)
    {
        return (condition & ifTrue) | (andNot(condition, ifFalse));
    }


    ALIGN_STACK_SSE inline static Int16x8 max (const Int16x8 &left, const Int16x8 &right) {
        return Int16x8(vmaxq_s16(left.data, right.data));
    }

    ALIGN_STACK_SSE inline static Int16x8 min (const Int16x8 &left, const Int16x8 &right) {
        return Int16x8(vminq_s16(left.data, right.data));
    }
};

template<>
inline Int16x8 SSEMath::divU<2>(const Int16x8 &val) {
    return  val >> 1;
}

template<>
inline Int16x8 SSEMath::divU<4>(const Int16x8 &val) {
    return val >> 2;
}

template<>
inline Int16x8 SSEMath::divU<16>(const Int16x8 &val) {
    return val >> 4;
}

template<>
inline Int16x8 SSEMath::div<2>(const Int16x8 &val) {
    return shiftArithmetic(val, 1);
}

template<>
inline Int16x8 SSEMath::div<4>(const Int16x8 &val) {
    return shiftArithmetic(val, 2);
}

template<>
inline Int16x8 SSEMath::div<16>(const Int16x8 &val) {
    return shiftArithmetic(val, 4);
};



template<>
inline Int16x8 SSEMath::mul<5>(const Int16x8 &val) {
    return (val << 2) + val;
}

template<>
inline UInt16x8 SSEMath::mul<5>(const UInt16x8 &val) {
    return (val << 2) + val;
}

template<>
inline Int16x8 SSEMath::div<5>(const Int16x8 &val) {
    Int16x8 a =  shrAdd(val >> 1, val, 2);
    a += shrAdd(shrAdd(a >> 4, a, 8), a, 12);
    a >>= 2;
    Int16x8 diff = val - mul<5>(a);
    a -= (diff > Int16x8((uint16_t)4));
    return a;
}

template<>
inline UInt16x8 SSEMath::div<5>(const UInt16x8 &val) {
    UInt16x8 a =  shrAdd(val >> 1, val, 2);
    a += shrAdd(shrAdd(a >> 4, a, 8), a, 12);
    a >>= 2;
    UInt16x8 diff = val - mul<5>(a);
    a -= (diff > UInt16x8((uint16_t)4));
    return a;
}

template<>
inline Int16x8 SSEMath::div<10>(const Int16x8 &val) {
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
inline Int16x8 SSEMath::mul<3>(const Int16x8 &val) {
    return (val << 1) + val;
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
#endif /* NEON_MATH_H_ */

