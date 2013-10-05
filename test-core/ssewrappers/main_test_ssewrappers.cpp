/**
 * \file main_test_ssewrappers.cpp
 * \brief This is the main file for the test ssewrappers
 *
 * \date Sep 25, 2010
 * \author alexander
 *
 */
#include <iostream>

#ifndef ASSERTS
#define ASSERTS
#endif

#include "global.h"

#include "sse_trace.h"
#include "sseWrapper.h"
#include "preciseTimer.h"
#include "readers.h"
#include "genericMath.h"

using namespace std;
using namespace corecvs;

#ifdef WITH_SSE

/* Test reverse function in 16x8 types */
void testSSEWrapperReverse (void)
{
    uint16_t data[8] = {1,2,3,4,5,6,7,8};
    uint16_t dataRes[8];
    Int16x8 sse(data);
    sse.reverse();
    sse.save(dataRes);

    for (unsigned i = 0; i < 8; i++)
    {

        ASSERT_TRUE_P(data[i] == dataRes[7-i], ("Reversing error:%i [%d != %d]", i, data[i], dataRes[7-i]));
        cout << dataRes[i] << " ";
    }

    cout << endl;
}

void testSSEWrapper (void)
{
    uint64_t LIMIT = 5;
    __m128i acc0 = _mm_set1_epi32(128);
    for (uint64_t i = 0 ; i < LIMIT; i++ )
    {
        int a = i * 32 / LIMIT;
        __m128i a0 = _mm_set1_epi32(a);
        __m128i b0 = _mm_set1_epi32(i % 2);
        __m128i c0 = _mm_sll_epi32(a0,b0);
        __m128i d0 = _mm_slli_epi32(a0,1);

        acc0 = _mm_add_epi32(acc0,d0);
        acc0 = _mm_add_epi32(acc0,c0);
        acc0 = _mm_sub_epi32(acc0,a0);
    }
    Int32x4 acc1(128);
    for (uint64_t i = 0 ; i < LIMIT; i++ )
    {
        int a = i * 32 / LIMIT;
        Int32x4 a1(a);
        Int32x4 b1((uint32_t)i % 2);
        Int32x4 c1 = a1 << b1;
        Int32x4 d1 = a1 << 1;

        acc1 += d1;
        acc1 += c1;

        acc1 -= a1;
    }
    ASSERT_TRUE(sse32(acc0, 0) ==  sse32(acc1.data, 0), "Ops... arithmetics flaw" );
}
#endif

void profileSSEWrapper (void)
{
#ifdef WITH_SSE
    uint64_t LIMIT = 10000000;

    PreciseTimer start;
    uint64_t delay0, delay1;

    start = PreciseTimer::currentTime();
    __m128i acc0 = _mm_set1_epi32(128);
    for (uint64_t i = 0 ; i < LIMIT; i++ )
    {
        int a = i * 32 / LIMIT;
        __m128i a0 = _mm_set1_epi32(a);
        __m128i b0 = _mm_set1_epi32(i % 2);
        __m128i c0 = _mm_sll_epi32(a0,b0);
        __m128i d0 = _mm_slli_epi32(a0,1);

        acc0 = _mm_add_epi32(acc0,d0);
        acc0 = _mm_add_epi32(acc0,c0);
        acc0 = _mm_sub_epi32(acc0,a0);
    }

    delay0 = start.usecsToNow();
    printf("Dirty Oldstyle :%8" PRIu64 "us \n", delay0);

    /* Same with new style */

    start = PreciseTimer::currentTime();
    Int32x4 acc1(128);
    for (uint64_t i = 0 ; i < LIMIT; i++ )
    {
        int a = i * 32 / LIMIT;
        Int32x4 a1(a);
        Int32x4 b1((uint32_t)i % 2);
        Int32x4 c1 = a1 << b1;
        Int32x4 d1 = a1 << 1;

        acc1 += d1;
        acc1 += c1;
        acc1 -= a1;
    }

    delay1 = start.usecsToNow();
    printf("Cool new style :%8" PRIu64 "us \n", delay1);
    printf("Diff is  :%8" PRIi64 "us \n", delay1 - delay0);

    printf("Results are %s\n",sse32(acc0, 0) ==  sse32(acc1.data, 0) ? "equal" : "different");

    ASSERT_TRUE(sse32(acc0, 0) ==  sse32(acc1.data, 0), "Ops... arithmetics flaw" );
#endif
}

void testBasicArithmetics ( void )
{
#ifdef WITH_SSE
    //2763 3070 3377 3684 3991
    //0xFFF

    uint16_t a[8] = {0xFFF, 0,0,0, 0,0,0,0};
    uint16_t b[8] = {0xFFF, 0,0,0, 0,0,0,0};
    uint16_t c[8] = {0xFFF, 0,0,0, 0,0,0,0};
    uint16_t d[8] = {0xFFF, 0,0,0, 0,0,0,0};
    uint16_t e[8] = {0xFFF, 0,0,0, 0,0,0,0};

    printf ("Blur Input: [%d %d %d %d %d]\n", a[0], b[0], c[0] ,d[0] ,e[0]);
    uint16_t result = (
            a[0] +
            2 * b[0] +
            4 * c[0] +
            2 * d[0] +
            e[0]);
    printf ("Wsum: %d\n", result);
    result = result / 10;
    printf ("Normal Blur: %d\n", result);

    Int16x8 sa(a);
    Int16x8 sb(b);
    Int16x8 sc(c);
    Int16x8 sd(d);
    Int16x8 se(e);
    printf ("Blur Input: [%d %d %d %d %d]\n", sa[0], sb[0], sc[0] ,sd[0] ,se[0]);


    Int16x8 sresult =
            sb + SSEMath::mul<2>(sc) + sd;

    printf ("Wsum: %d\n", sresult[0]);
    sresult = SSEMath::div<2>(sa + se) + sresult;
    printf ("Wsum2: %d\n", sresult[0]);
    sresult = Int16x8(SSEMath::div<5>(UInt16x8(sresult)));
    printf ("Wsum10: %d\n", sresult[0]);
    printf ("SSE Blur: %d\n", sresult[0]);
#endif
}

void testSignUnsign16 ( void )
{
#ifdef WITH_SSE
    cout << "RUNNING:testSignUnsign";
    //2763 3070 3377 3684 3991
    //0xFFF

    //5   312   619 2212  2519  2826 323   630   937

    int16_t a00t[8] = {    5, 0,0,0, 0,0,0,0};
    int16_t a01t[8] = {  312, 0,0,0, 0,0,0,0};
    int16_t a02t[8] = {  619, 0,0,0, 0,0,0,0};

    int16_t a10t[8] = { 2212, 0,0,0, 0,0,0,0};
    int16_t a11t[8] = { 2519, 0,0,0, 0,0,0,0};
    int16_t a12t[8] = { 2826, 0,0,0, 0,0,0,0};

    int16_t a20t[8] = {  323, 0,0,0, 0,0,0,0};
    int16_t a21t[8] = {  630, 0,0,0, 0,0,0,0};
    int16_t a22t[8] = {  937, 0,0,0, 0,0,0,0};


    Int16x8 a00(a00t);
    Int16x8 a01(a01t);
    Int16x8 a02(a02t);

    Int16x8 a10(a10t);
    Int16x8 a11(a11t);
    Int16x8 a12(a12t);

    Int16x8 a20(a20t);
    Int16x8 a21(a21t);
    Int16x8 a22(a22t);

    printf ("Loading result:\n%d %d  %d\n%d %d  %d\n%d %d  %d\n",
            a00[0], a01[0], a02[0],
            a10[0], a11[0], a12[0],
            a20[0], a21[0], a22[0]
        );

   /* Int16x8 center = SSEMath::mul<2>(a10 - a12);
    printf ("center: %d\n", center[0]);

    Int16x8 result = SSEMath::div<4>(a00 + a20 - a02 - a22 + center);
    printf ("result: %d\n", result[0]);*/

    Int16x8 positive = Int16x8(a00 + a20 + SSEMath::mul<2>(a10));
    Int16x8 negative = Int16x8(a02 + a22 + SSEMath::mul<2>(a12));

    UInt16x8 result = UInt16x8(SSEMath::div<4>( positive - negative ) + Int16x8((int16_t)0));
    cout << result << endl;

    int16_t positiveS = int16_t(a00t[0] + a20t[0] + GenericMath<uint16_t>::mul<2>(a10t[0]));
    int16_t negativeS = int16_t(a02t[0] + a22t[0] + GenericMath<uint16_t>::mul<2>(a12t[0]));

    uint16_t resultS = uint16_t(GenericMath<uint16_t>::div<4>( positiveS - negativeS ) + int16_t((int16_t)0));
    cout << resultS << endl;



#endif
}

void testAdditionalFunctions()
{
#ifdef WITH_SSE
    cout << "Starting testAdditionalFunctions" << endl;
    int16_t a0t[8] = { -8700, 8700,0,0, 0,0,0,0};
    int16_t a1t[8] = {  2207, 0,0,0, 0,0,0,0};

    Int16x8 a0(a0t);
    Int16x8 a1(a1t);

    int16_t b0 = a0t[0];
//    int16_t b1 = a1t[0];

    /*Testing ABS*/
    Int16x8 absa0 = SSEMath::abs(a0);
    cout << "SSEMath::abs()=" << absa0 << endl;
    ASSERT_TRUE_P(absa0[0] ==  8700, ("Wrong SSE abs result 0"));
    ASSERT_TRUE_P(absa0[1] ==  8700, ("Wrong SSE abs result 1"));

    int16_t absb0 = GenericMath<uint16_t>::abs(b0);
    cout << "GenericMath<uint16_t>::abs()=" << absb0 << endl;
    ASSERT_TRUE_P(absb0 == 8700, ("Wrong GenericMath abs result"));

    /* Testing max */
    cout << "Finished" << endl;
#endif
}

#ifdef WITH_SSE
void testRGBReader(void)
{
    cout << "Starting testRGBReader..." << endl;
    uint8_t data[4 * 16];
    for (unsigned i = 0; i < 16; i++)
    {
        for (unsigned j = 0; j < 4; j++)
        {
            data[i * 4 + j] = (j * 16) + i;
            cout << (int)data[i*4 + j] << " ";
        }
        cout << endl;
    }
    FixedVector<Int8x16,4> r = SSEReader16BBBB_BBBB::read((uint32_t *)data);
    cout << r[0] << endl;
    cout << r[1] << endl;
    cout << r[2] << endl;
    cout << r[3] << endl;

    uint8_t outData[4 * 16];

    SSEReader16BBBB_BBBB::write(r, (uint32_t *)outData);

    cout << "Result written back" << endl;
    for (unsigned i = 0; i < 16; i++)
    {
        for (unsigned j = 0; j < 4; j++)
        {
            cout << (int)outData[i*4 + j] << " ";
        }
        cout << endl;
    }
    cout << "Finished" << endl;
}

void testExtendingRGBReader(void)
{
    uint8_t data[4 * 8];
    for (unsigned i = 0; i < 8; i++)
    {
        for (unsigned j = 0; j < 4; j++)
        {
            data[i * 4 + j] = (j * 16) + i;
            cout << (int)data[i*4 + j] << " ";
        }
        cout << endl;
    }
    FixedVector<Int16x8,4> r = SSEReader8BBBB_DDDD::read((uint32_t *)data);

    cout << r[0] << endl;
    cout << r[1] << endl;
    cout << r[2] << endl;
    cout << r[3] << endl;

    uint8_t outData[4 * 8];

    SSEReader8BBBB_DDDD::write(r, (uint32_t *)outData);

    cout << "Result written back" << endl;
    for (unsigned i = 0; i < 8; i++)
    {
        for (unsigned j = 0; j < 4; j++)
        {
            cout << (int)outData[i*4 + j] << " ";
            ASSERT_TRUE_P(data[i*4 + j] == outData[i*4 + j], ("Error"));
        }
        cout << endl;
    }
    cout << "Finished" << endl;
}


void testExtendingReader(void)
{
    uint16_t data[2 * 8];
    for (unsigned i = 0; i < 8; i++)
    {
        for (unsigned j = 0; j < 2; j++)
        {
            data[i * 2 + j] = (j * 16) + i;
            cout << (int)data[i * 2 + j] << " ";
        }
        cout << endl;
    }
    FixedVector<Int16x8,2> r = SSEReader8DD_DD::read((uint32_t *)data);
    cout << r[0] << endl;
    cout << r[1] << endl;

    uint16_t outData[2 * 8];

    SSEReader8DD_DD::write(r, (uint32_t *)outData);

    cout << "Result written back" << endl;
    for (unsigned i = 0; i < 8; i++)
    {
        for (unsigned j = 0; j < 2; j++)
        {
            cout << (int)outData[i*2 + j] << " ";
        }
        cout << endl;
    }
    cout << "Finished" << endl;
}

#endif

int main (int /*argC*/, char ** /*argV*/)
{

#ifdef WITH_SSE
    testSignUnsign16 ( );
    testAdditionalFunctions();
    testRGBReader();
    testExtendingRGBReader();
    testExtendingReader();
    testSSEWrapperReverse();
    testSSEWrapper ();
    testBasicArithmetics();
    return 0;
    profileSSEWrapper();
#endif
    cout << "PASSED" << endl;
    return 0;
}
