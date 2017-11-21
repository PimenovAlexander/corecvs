/**
 * \file main_test_arithmetics.cpp
 * \brief This is the main file for the test arithmetics
 *
 * \date Sep 22, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#ifdef WITH_SSE
#include <emmintrin.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/utils/sse_trace.h"
#include "core/utils/preciseTimer.h"
#include "core/math/sse/sseWrapper.h"

using namespace corecvs;

#ifdef WITH_SSE
ALIGN_STACK_SSE inline __m128i sse_div_u32_by_25(__m128i n) {
    __m128i q = _mm_add_epi32(_mm_srli_epi32(n,1),_mm_srli_epi32(n,3));
    q = _mm_add_epi32(q,_mm_srli_epi32(q,6));
    q = _mm_add_epi32(_mm_add_epi32(q,_mm_srli_epi32(q,7)),
                      _mm_add_epi32(_mm_srli_epi32(n,12),_mm_srli_epi32(n,15)));
    q = _mm_add_epi32(q,_mm_srli_epi32(q,20));
    q = _mm_srli_epi32(q,4);

    // r = (n - (q * 16)) - (q + q * 8) = n - 25 * q;
    __m128i r = _mm_sub_epi32(_mm_sub_epi32(n,_mm_slli_epi32(q,4)),_mm_add_epi32(q,_mm_slli_epi32(q,3))); // q*25
    return _mm_sub_epi32(q,_mm_cmpgt_epi32(r,_mm_set1_epi32(24))); // r>=25 -> q - (-1)
}

ALIGN_STACK_SSE inline __m128i sse_div_u32_by_25_fast(__m128i n) {
    __m128i inverse = _mm_set1_epi16   (static_cast<short>(0xA3D7));
    __m128i shifted = _mm_srli_epi32  (n, 1);
    /* Now high 16 bits of n are empty*/
    __m128i mulhi   = _mm_mulhi_epu16(inverse, shifted);
    __m128i q       = _mm_srli_epi32  (mulhi,3);

    // r = (n - (q * 16)) - (q + q * 8) = n - 25 * q;
    __m128i r = _mm_sub_epi32(_mm_sub_epi32(n,_mm_slli_epi32(q,4)),_mm_add_epi32(q,_mm_slli_epi32(q,3))); // q*25
    return _mm_sub_epi32(q,_mm_cmpgt_epi32(r,_mm_set1_epi32(24))); // r>=25 -> q - (-1)
}

ALIGN_STACK_SSE inline __m128i sse_div_u32_by_25_fast_fp(__m128i n) {

    __m128 div25 = _mm_cvtepi32_ps (_mm_set1_epi32(25));
    __m128 fp = _mm_cvtepi32_ps (_mm_sub_epi32(n, _mm_set1_epi32(12)));
    __m128 fr = _mm_div_ps(fp,div25);
    return _mm_cvtps_epi32(fr);
}

ALIGN_STACK_SSE inline __m128i sse_div_u32_by_25_fast_fpn(__m128i n) {
    __m128 div25 = _mm_cvtepi32_ps (_mm_set1_epi32(25));
    __m128 fp = _mm_cvtepi32_ps (n);
    __m128 fr = _mm_div_ps(fp,div25);
    return _mm_cvtps_epi32(fr);
}

ALIGN_STACK_SSE inline __m128i sse_div_u32_by_25_fp(__m128i n) {
    return _mm_cvttps_epi32(_mm_mul_ps(_mm_cvtepi32_ps(n),_mm_set1_ps(1.0f/25)));
}

inline uint32_t divideBy25_32 (uint32_t in)
{
    uint32_t in0_101 = (in >> 1)  + (in >> 3);
    uint32_t in0_111 = in0_101 + (in >> 2);
    uint32_t in0_00001010001111010111 = ((in0_101) + (in0_101 >> 9) + (in0_111 >> 6) + (in0_111 >> 13)) >> 4;
    uint32_t result = (in0_00001010001111010111) + (in0_00001010001111010111 >> 20);

    uint16_t r = in - result * 25;
    if (r >= 25) result++;
    if (r >= 50) result++;
    return result;
}

/**
 * As 1/25 is
 *   0b0.(00001010001111010111)
 *
 * The period is presented as follows:
 *
 * |1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20|21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40
 * |0  0  0  0  1  0  1  0  0  0  1  1  1  1  0  1  0  1  1  1| 0  0  0  0  1  0  1  0  0  0  1  1  1  1  0  1  0  1  1  1
 * |           |     A     |     3     |    D      |     7    |
 *
 *
 * the division result could be computed as the addition of the shifted values.
 *
 * This version is for 16bit
 *
 */
inline uint32_t divideBy25_32_fast (uint32_t in)
{
    uint32_t mul1    = 0xA3D7 * in;
    uint32_t result  = (mul1 >> 20);
    uint16_t r = in - result * 25;
    result += (r >= 25) ? 1 : 0;
    return result;
}

inline uint32_t divideBy25_32_fast1 (uint32_t in)
{
    uint32_t mul1    = 0xA3D7 * (in >> 1);
    uint32_t result  = (mul1 >> 19);
    uint16_t r = in - result * 25;
    result += (r >= 25) ? 1 : 0;
    return result;
}


/**
 * As 1/5 is
 *   0b0.(0011)
 *
 * The period is presented as follows:
 *
 * |1  2  3  4 |5  6  7  8 |
 * |0  0  1  1  0  0  1  1
 * |     3     |     3     |
 *
 *  following presentation possible
 *
 *   (3 / 16)   - 1/5 = -0.0125
 *   (27 / 256) - 1/5 = -0.00078125
 *
 * the division result could be computed as the addition of the shifted values.
 *
 * This version is for 16bit
 *
 */

uint16_t divby10(uint16_t value)
{
    value >>= 1;
    /*now divide by 5*/
    /*uint16_t a = (value >> 3)  + (value >> 4);
    uint16_t result = a + (a >> 4) + (a >> 12);*/

    //uint32_t a = ((uint32_t )0x3333 * value) >> 16;

    /*uint16_t a = value * 3;
    uint16_t result = (a >> 4) + (a >> 8) + (a >> 12);*/

    /*uint16_t a = (value >> 3) + (value >> 4);
    uint16_t result = a + (a >> 4) + (a >> 8) + (a >> 12);*/

    uint16_t a = (value * 3);
    uint16_t result = a;
    for (int i = 0; i < 3; i++) {
         a = (a >> 4); result += a;
    }
    result >>=4;


    /*uint32_t b = value;
    uint16_t result = (b * 0x3333) >> 16;*/

   // uint16_t result = ((uint32_t )0x3333 * value) >> 16;
    if (result * 5 > value) result--;

    return result;
}

ALIGN_STACK_SSE
TEST(Arithmetics, testDivisionBy25)
{
    unsigned LIMIT = 0x1000 * 25;
//    unsigned LIMIT = 150;

    for (unsigned i = 0; i < LIMIT; i++)
    {
        __m128i input   = _mm_set1_epi32(i);
        __m128i result  = sse_div_u32_by_25(input);
        __m128i result1 = sse_div_u32_by_25_fast(input);

/*        int wasMode = _MM_GET_ROUNDING_MODE();
        _MM_SET_ROUNDING_MODE(_MM_ROUND_DOWN);
        __m128i result3 = sse_div_u32_by_25_fast_fpn(input);
        _MM_SET_ROUNDING_MODE(wasMode);*/

        __m128i result4 = sse_div_u32_by_25_fp(input);

        unsigned div = i / 25;
        unsigned div1 = divideBy25_32(i);
        unsigned div2 = divideBy25_32_fast(i);
        unsigned div3 = divideBy25_32_fast1(i);

        //printf("%i %d %d\n",i, div, sse32(result2, 0));

        if (  sse32(result, 0) != div ||
              sse32(result1, 0) != div ||
//              sse32(result3, 0) != div ||
              sse32(result4, 0) != div ||
              div1 != div ||
              div2 != div ||
              div3 != div  ) {
            printf("Dividing      : %i\n", i);
            printf("Old SSE       : %d\n", sse32(result, 0));
            printf("Real Result   : %d\n", div);
            printf("Shifts Result : %d\n", div1);
            printf("Fast   Result : %d\n", div2);
            printf("Fast 16Result : %d\n", div3);
            printf("New SSE       : %d\n", sse32(result1, 0));
//            printf("FP SSE div    : %d\n", sse32(result3, 0));
            printf("FP SSE mul    : %d\n", sse32(result4, 0));

            ASSERT_TRUE(false);
            break;
        }
    }
}

ALIGN_STACK_SSE
TEST(Arithmetics, profileDivisionBy25)
{
    const unsigned  LIMIT = 50000;
    PreciseTimer start;
    uint64_t delay;

    __m128i rOld;
    __m128i rFast;
    __m128i rFp;

    printf("Profiling Old /25  :");
    start = PreciseTimer::currentTime();
    for (unsigned j = 0; j < LIMIT ; j++)
        for (unsigned i = 0; i < 0x1000 * 25; i++)
        {
           __m128i input   = _mm_set1_epi32(i);
           __m128i result  = sse_div_u32_by_25(input);
           rOld = result;
        }
    delay = start.usecsToNow();
    printf("%9" PRIu64 "us\n", delay);

    printf("Profiling _fast /25:");
    start = PreciseTimer::currentTime();
    for (unsigned j = 0; j < LIMIT ; j++)
        for (unsigned i = 0; i < 0x1000 * 25; i++)
        {
           __m128i input   = _mm_set1_epi32(i);
           __m128i result1 = sse_div_u32_by_25_fast(input);
           rFast = result1;
        }
    delay = start.usecsToNow();
    printf("%9" PRIu64 "us\n", delay);

    printf("Profiling _fp /25  :");
    start = PreciseTimer::currentTime();
    for (unsigned j = 0; j < LIMIT ; j++)
        for (unsigned i = 0; i < 0x1000 * 25; i++)
        {
           __m128i input   = _mm_set1_epi32(i);
           __m128i result1 = sse_div_u32_by_25_fp(input);
           rFp = result1;
        }
    delay = start.usecsToNow();
    printf("%9" PRIu64 "us\n", delay);


    std::cout << Int32x4(rOld)  << std::endl;
    std::cout << Int32x4(rFast) << std::endl;
    std::cout << Int32x4(rFp)   << std::endl;

    printf("And once again in reverse order:\n");
    _MM_SET_ROUNDING_MODE(_MM_ROUND_DOWN);
    printf("Profiling FLOAT DIV /25:");
    start = PreciseTimer::currentTime();
    for (unsigned j = 0; j < LIMIT ; j++)
        for (unsigned i = 0; i < 0x1000 * 25; i++)
        {
            __m128i input   = _mm_set1_epi32(i);
            __m128i result1 = sse_div_u32_by_25_fast_fpn(input);
            result1 = result1;
         }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us\n", delay);
    _MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
    printf("Profiling FLOAT MUL /25:");
    start = PreciseTimer::currentTime();
    for (unsigned j = 0; j < LIMIT ; j++)
        for (unsigned i = 0; i < 0x1000 * 25; i++)
        {
            __m128i input   = _mm_set1_epi32(i);
            __m128i result1 = sse_div_u32_by_25_fp(input);
            result1 = result1;
        }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us\n", delay);

    printf("Profiling _fast /25    :");
    start = PreciseTimer::currentTime();
    for (unsigned j = 0; j < LIMIT ; j++)
        for (unsigned i = 0; i < 0x1000 * 25; i++)
        {
           __m128i input   = _mm_set1_epi32(i);
           __m128i result1 = sse_div_u32_by_25_fast(input);
           result1 = result1;
        }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us\n", delay);

    printf("Profiling Old /25:");
    start = PreciseTimer::currentTime();
    for (unsigned j = 0; j < LIMIT ; j++)
        for (unsigned i = 0; i < 0x1000 * 25; i++)
        {
           __m128i input   = _mm_set1_epi32(i);
           __m128i result  = sse_div_u32_by_25(input);
           result = result;
        }
    delay = start.usecsToNow();
    printf("%8" PRIu64 "us\n", delay);
}
#endif // WITH_SSE

TEST(Arithmetics, testDivisionBy10)
{
    /*for (uint16_t i = 0; i < 0x3FFF; i++)
    {
        uint16_t  div = divby10(i);
        printf("%u %u %d\n", i, div, ((int)i / 10) - div);
    }*/

    for (int delay = 0; delay < 0x3FFF; delay++)
    {
        short int a = delay;
        /*short int b;*/
#ifdef BUF_DUMP
        char buffer[10];
#endif

        int pos = 0;
        for (; pos < 5; pos++)
            if (a != 0) {
                 uint16_t a2 = a >> 1;
                 uint16_t b = (a2 * 3);
                 uint16_t result = b;
                 for (int i = 0; i < 3; i++) {
                     b = (b >> 4); result += b;
                 }
                 result >>= 4;

                 uint16_t check = result * 10;
                 if (check > a) {result--; check -= 10;}
                 if (check <= a - 10) {result++; check += 10;}

#ifdef BUF_DUMP
                 buffer[pos] = a - check + '0';
#endif
                 a = result;
            } else {
                break;
            }
#ifdef BUF_DUMP
        buffer[pos++] = 0;

      printf("%d == %s\n", delay, buffer);
        //    6 ==    6
        //   10 ==   01
        //   19 ==   91
        //   20 ==   02
        //   92 ==   92
        //   99 ==   99
        //  100 ==  001
        //  101 ==  101
        //  109 ==  901
        //  591 ==  195
        // 1023 == 3201
        // ...
#endif
    }
}

