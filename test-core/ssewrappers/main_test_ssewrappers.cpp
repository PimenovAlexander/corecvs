/**
 * \file main_test_ssewrappers.cpp
 * \brief This is the main file for the test ssewrappers
 *
 * \date Sep 25, 2010
 * \author alexander
 *
 */
#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/utils/sse_trace.h"

#include "core/math/sse/sseWrapper.h"
#include "core/math/neon/neonWrapper.h"

#include "core/utils/preciseTimer.h"
#include "core/buffers/kernels/fastkernel/readers.h"
#include "core/math/generic/genericMath.h"

//#include "core/math/avx/int32x8.h"

using namespace corecvs;


using std::istream;
using std::ostream;
using std::cout;

#ifdef WITH_SSE

TEST(SSEWrappers, testSSEWrapper)
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
    CORE_ASSERT_TRUE(sse32(acc0, 0) == sse32(acc1.data, 0), "Ops... arithmetics flaw");
}

#endif // WITH_SSE

TEST(SSEWrappers, profileSSEWrapper)
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

    CORE_ASSERT_TRUE(sse32(acc0, 0) == sse32(acc1.data, 0), "Ops... arithmetics flaw");
#endif
}

TEST(SSEWrappers, testSaturatedArithmetics)
{
#ifdef WITH_SSE
    int8_t a0t[16] = {  55, 100, 120, -100, -80,  10,  10,  10, 0, 0, 0, 0, 0, 0, 0, 0};
    int8_t a1t[16] = {  55, 100, 120, -100, -80, 120, 120, 120, 0, 0, 0, 0, 0, 0, 0, 0};

    Int8x16 A0(a0t);
    Int8x16 A1(a1t);

    Int8x16 sSum = adds(A0, A1);
    Int8x16 sDif = subs(A0, A1);

    cout << "Signed Sum :" << sSum << endl;
    cout << "Signed Diff:" << sDif << endl;

    uint8_t b0t[16] = {  55, 100, 150, 200, 250,  10,  10,  10, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t b1t[16] = {  55, 100, 150, 200, 250, 100, 100, 100, 0, 0, 0, 0, 0, 0, 0, 0};

    UInt8x16 B0(b0t);
    UInt8x16 B1(b1t);

    UInt8x16 uSum = adds(B0, B1);
    UInt8x16 uDif = subs(B0, B1);

    cout << "Unsigned Sum :" << uSum << endl;
    cout << "Unsigned Diff:" << uDif << endl;
#endif
}



TEST(SSEWrappers, testAdditionalFunctions)
{
#ifdef WITH_SSE
    int16_t a0t[8] = { -8700, 8700,0,0, 0,0,0,0};
    int16_t a1t[8] = {  2207, 0,0,0, 0,0,0,0};

    Int16x8 a0(a0t);
    Int16x8 a1(a1t);

    int16_t b0 = a0t[0];
//    int16_t b1 = a1t[0];

    /*Testing ABS*/
    Int16x8 absa0 = SSEMath::abs(a0);
    cout << "SSEMath::abs()=" << absa0 << endl;
    CORE_ASSERT_TRUE_P(absa0[0] == 8700, ("Wrong SSE abs result 0"));
    CORE_ASSERT_TRUE_P(absa0[1] == 8700, ("Wrong SSE abs result 1"));

    int16_t absb0 = GenericMath<uint16_t>::abs(b0);
    cout << "GenericMath<uint16_t>::abs()=" << absb0 << endl;
    CORE_ASSERT_TRUE_P(absb0 == 8700, ("Wrong GenericMath abs result"));
#endif // WITH_SSE
}

#ifdef WITH_SSE

TEST(SSEWrappers, testRGBReader)
{
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
}

TEST(SSEWrappers, testExtendingRGBReader)
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
            CORE_ASSERT_TRUE_P(data[i * 4 + j] == outData[i * 4 + j], ("Error"));
        }
        cout << endl;
    }
}

TEST(SSEWrappers, testDoubleExtendingRGBReader)
{
    uint8_t data[8 * 8];
    for (unsigned i = 0; i < 8; i++)
    {
        for (unsigned j = 0; j < 8; j++)
        {
            data[i * 8 + j] = (j * 10) + i;
            cout << (int)data[i*8 + j] << " ";
        }
        cout << endl;
    }
    FixedVector<Int16x8,8> r = SSEReader8BBBBBBBB_DDDDDDDD::read((int64_t *)data);

    for (unsigned i = 0; i < 8; i++)
    {
//        int st = i * 10;
        cout << r[i] << endl;
//        r[i] = Int16x8(st, st + 1, st + 2, st + 3, st + 4, st + 5, st + 6, st + 7);
    }

    cout << "Input" << endl;
    for (unsigned i = 0; i < 8; i++)
    {
        cout << r[i] << endl;
    }


    uint8_t outData[8 * 8];

    SSEReader8BBBBBBBB_DDDDDDDD::write(r, (uint32_t *)outData);

    cout << "Result written back" << endl;
    for (unsigned i = 0; i < 8; i++)
    {
        for (unsigned j = 0; j < 8; j++)
        {
            cout << (int)outData[i*8 + j] << " ";
//            CORE_ASSERT_TRUE_P(data[i*4 + j] == outData[i*4 + j], ("Error"));
        }
        cout << endl;
    }
}

TEST(SSEWrappers, testDoubleExtendingRGBReader1)
{
    uint8_t data[8 * 8];
    for (unsigned i = 0; i < 8; i++)
    {
        for (unsigned j = 0; j < 8; j++)
        {
            data[i * 8 + j] = (j * 10) + i;
            cout << (int)data[i*8 + j] << " ";
        }
        cout << endl;
    }

    int64_t *ptr = (int64_t *)data;

    /* Preparing 2 registers that contain...*/
    /* | A0 B0 C0 D0  E0 F0 G0 H0 | A1 B1 C1 D1 E1 F1 G1 H1 | */
    Int64x2 a(ptr      );
    /* | A2 B2 C2 D2  E2 F2 G2 H2 | A3 B3 C3 D3 E3 F3 G3 H3 | */
    Int64x2 b(ptr + 0x2);
    /* | A4 B4 C4 D4  E4 F4 G4 H4 | A5 B5 C5 D5 E5 F5 G5 H5 | */
    Int64x2 c(ptr + 0x4);
    /* | A6 B6 C6 D6  E6 F6 G6 H6 | A7 B7 C7 D7 E7 F7 G7 H7 | */
    Int64x2 d(ptr + 0x6);

    cout << "a =" << Int8x16(a.data) << endl;
    cout << "b =" << Int8x16(b.data) << endl;
    cout << "c =" << Int8x16(c.data) << endl;
    cout << "d =" << Int8x16(d.data) << endl;

    cout << " " << endl;
    Int64x2 e = Int32x4::unpackLower(Int32x4(a.data), Int32x4(b.data));
    cout << "upackl =" << Int8x16(e.data) << endl;
    Int64x2 g = Int32x4::unpackHigher(Int32x4(a.data), Int32x4(b.data));
    cout << "upackh =" << Int8x16(g.data) << endl;
    Int64x2 e1 = Int32x4::unpackLower(Int32x4(c.data), Int32x4(d.data));
    cout << "upackl =" << Int8x16(e1.data) << endl;
    Int64x2 g1 = Int32x4::unpackHigher(Int32x4(c.data), Int32x4(d.data));
    cout << "upackh =" << Int8x16(g1.data) << endl;

    cout << "interl 8" << endl;
    Int16x8 u = Int8x16::unpackLower(Int8x16(e.data), Int8x16(g.data));
    cout << "interl =" << Int8x16(u.data) << endl;
    Int16x8 v = Int8x16::unpackHigher(Int8x16(e.data), Int8x16(g.data));
    cout << "interh =" << Int8x16(v.data) << endl;
    Int16x8 u1 = Int8x16::unpackLower(Int8x16(e1.data), Int8x16(g1.data));
    cout << "interl =" << Int8x16(u1.data) << endl;
    Int16x8 v1 = Int8x16::unpackHigher(Int8x16(e1.data), Int8x16(g1.data));
    cout << "interh =" << Int8x16(v1.data) << endl;

    cout << "unpack again" << endl;
    Int64x2 k = Int32x4::unpackLower(Int32x4(u.data), Int32x4(v.data));
    cout << "upackl =" << Int8x16(k.data) << endl;
    Int64x2 l = Int32x4::unpackHigher(Int32x4(u.data), Int32x4(v.data));
    cout << "upackh =" << Int8x16(l.data) << endl;
    Int64x2 k1 = Int32x4::unpackLower(Int32x4(u1.data), Int32x4(v1.data));
    cout << "upackl =" << Int8x16(k1.data) << endl;
    Int64x2 l1 = Int32x4::unpackHigher(Int32x4(u1.data), Int32x4(v1.data));
    cout << "upackh =" << Int8x16(l1.data) << endl;

    cout << "interl 16" << endl;
    Int32x4 s = Int16x8::unpackLower(Int16x8(k.data), Int16x8(l.data));
    cout << "interl =" << Int8x16(s.data) << endl;
    Int32x4 t = Int16x8::unpackHigher(Int16x8(k.data), Int16x8(l.data));
    cout << "interh =" << Int8x16(t.data) << endl;
    Int32x4 s1 = Int16x8::unpackLower(Int16x8(k1.data), Int16x8(l1.data));
    cout << "interl =" << Int8x16(s1.data) << endl;
    Int32x4 t1 = Int16x8::unpackHigher(Int16x8(k1.data), Int16x8(l1.data));
    cout << "interh =" << Int8x16(t1.data) << endl;

    cout << "interl 32" << endl;
    Int64x2 x = Int32x4::unpackLower(Int32x4(s.data), Int32x4(s1.data));
    cout << "interl =" << Int8x16(x.data) << endl;
    Int64x2 y = Int32x4::unpackHigher(Int32x4(s.data), Int32x4(s1.data));
    cout << "interh =" << Int8x16(y.data) << endl;
    Int64x2 x1 = Int32x4::unpackLower(Int32x4(t.data), Int32x4(t1.data));
    cout << "interl =" << Int8x16(x1.data) << endl;
    Int64x2 y1 = Int32x4::unpackHigher(Int32x4(t.data), Int32x4(t1.data));
    cout << "interh =" << Int8x16(y1.data) << endl;
}

TEST(SSEWrappers, testPack)
{
    Int32x4 a((int32_t)0xA);
    cout << "a =" << Int8x16(a.data) << endl;
    Int32x4 b((int32_t)0xB);
    cout << "b =" << Int8x16(b.data) << endl;

    Int16x8 f((int16_t)0);
    std::cout << "ppack =" << std::hex << Int8x16(f.data) << std::dec << std::endl;
    f = Int16x8::pack(a, b);
    std::cout << " pack =" << std::hex << Int8x16(f.data) << std::dec << std::endl;
}

TEST(SSEWrappers, testExtendingReader)
{
    uint16_t data[2 * 8];
    for (unsigned i = 0; i < 8; i++)
    {
        for (unsigned j = 0; j < 2; j++)
        {
            data[i * 2 + j] = (j * 16) + i;
            cout << (int)data[i * 2 + j] << " ";
        }
        std::cout << endl;
    }
    FixedVector<Int16x8,2> r = SSEReader8DD_DD::read((uint32_t *)data);
    std::cout << r[0] << std::endl;
    std::cout << r[1] << std::endl;

    uint16_t outData[2 * 8];

    SSEReader8DD_DD::write(r, (uint32_t *)outData);

    std::cout << "Result written back" << endl;
    for (unsigned i = 0; i < 8; i++)
    {
        for (unsigned j = 0; j < 2; j++)
        {
            std::cout << (int)outData[i * 2 + j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

TEST(SSEWrappers, test64bit)
{
    std::cout << "Testing 64 bit sse wrapper" << std::endl;
    std::cout << "Testing 64 bit to 16bit" << std::endl;

#if _MSC_VER != 1600    // TODO: for msvc2010 it doesn't linking, why?
    Int64x2 l64(0xFF);
    std::cout << "Lower byte in 64 bit" << l64 << std::endl;
#else
    Int64x2 l64;
    std::cout << "test64bit() FAILED on msvc2010" << std::endl;
    CORE_ASSERT_FAIL("STOP");
#endif

    UInt8x16 l8(l64.data);
    std::cout << "Lower byte in 8 bit" << l8 << std::endl;

    Int64x2 l64back(l8.data);
    std::cout << "Lower byte in 64 bit back" << l64back << std::endl;

    Int64x2 l64backeq = Int64x2(l8.data);
    std::cout << "Lower byte in 64 bit back =" << l64backeq << std::endl;

    UInt8x16 a8(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
    std::cout << "Test vector 8 bit" << a8 << std::endl;

    Int64x2 a64(a8.data);
    std::cout << "Test vector 64 bit" << a64 << std::endl;

    UInt8x16 a8back(a64.data);
    std::cout << "Test vector back in 16 bit" << a8back << std::endl;

    Int64x2 a64l64 = a64 & l64;
    std::cout << "And in 64 form presented in 64" << a64l64 << std::endl;

    UInt8x16 a64l8 = UInt8x16(a64l64.data);
    std::cout << "And in 64 form presented in 64" << a64l8 << std::endl;

    UInt8x16 a8l8 = a8 & l8;
    std::cout << "And in 8 form presented in 8" << a8l8 << std::endl;

    /*UInt8x16 al16 = UInt8x16(al.data);
    std::cout << al16 << std::endl;*/
    std::cout << std::endl;
}

#endif // WITH_SSE

#ifdef WITH_SSE
TEST(SSEWrappers, popcnt)
{
    unsigned int a, res;

    res = __builtin_popcount(a = 0x2F63A150);
    std::cout << "The value 0x" << std::hex << a << std::dec << " should have 14 ones :" << res << std::endl;
    CORE_ASSERT_TRUE_S(res == 14);

    res = __builtin_popcount(a = 0xAAAAAAAA);
    std::cout << "The value 0x" << std::hex << a << std::dec << " should have 16 ones :" << res << std::endl;
    CORE_ASSERT_TRUE_S(res == 16);
}
#endif

uintptr_t getMask(uintptr_t addr)
{
    uintptr_t a = 1;
    while (!(a & addr))
        a <<= 1;
    return a;
}

#if defined(WITH_SSE) || defined(WITH_NEON)

typedef Int32x4 TestAlignmentTarget;
class TestAlignmentHolder
{
public:
    TestAlignmentTarget bar;
};

TEST(SSEWrappers, AlignmentTest)
{
    uintptr_t alignment = 0;
    for (int i = 0; i < 10000; ++i)
    {
        alignment |= reinterpret_cast<uintptr_t>(&(new TestAlignmentHolder)->bar);
    }
    uintptr_t a = getMask(alignment);
    std::cout << "C++ standard in general " <<  (a >= 32 ? "rock-n-rolls" : "sucks") << " 'cause alignment is " << a  << std::endl;
}

#endif
