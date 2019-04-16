/**
 * \file main_test_vectorize.cpp
 * \brief This is the main file for the test vectorize 
 *
 * \date апр 16, 2018
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/sse/sseWrapper.h"
#include "core/math/neon/neonWrapper.h"
#include "core/math/generic/genericMath.h"


using namespace std;
using namespace corecvs;


/**
 *  Common
 *  SSE and NEON tests
 **/
#if defined(WITH_SSE) || defined(WITH_NEON)

/* Test reverse function in 16x8 types */
TEST(VectorWrappers, testSSEWrapperReverse)
{
    uint16_t data[8] = {1,2,3,4,5,6,7,8};
    uint16_t dataRes[8];
    Int16x8 sse(data);
    sse.reverse();
    sse.save(dataRes);

    for (unsigned i = 0; i < 8; i++)
    {
        CORE_ASSERT_TRUE_P(data[i] == dataRes[7 - i], ("Reversing error:%i [%d != %d]", i, data[i], dataRes[7 - i]));
        cout << dataRes[i] << " ";
    }
    cout << endl;
}

TEST(VectorWrappers, testSSEConstructor) 
{
     Int32xN n = Int32xN(0x7FFF7FFF);
     cout << n << endl;
}



TEST(VectorWrappers, testBasicArithmetics)
{
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
    ASSERT_EQ(result, 10 * 0xFFF);

    result = result / 10;
    printf ("Normal Blur: %d\n", result);
    ASSERT_EQ(result, 0xFFF);

    Int16x8 sa(a);
    Int16x8 sb(b);
    Int16x8 sc(c);
    Int16x8 sd(d);
    Int16x8 se(e);
    printf ("Blur Input: [%d %d %d %d %d]\n", sa[0], sb[0], sc[0] ,sd[0] ,se[0]);

    Int16x8 sresult =
            sb + SSEMath::mul<2>(sc) + sd;

    printf ("Wsum: %d\n", sresult[0]);
    ASSERT_TRUE(sresult[0] == 4 * 0xFFF);

    sresult = SSEMath::div<2>(sa + se) + sresult;
    printf ("Wsum2: %d\n", sresult[0]);
    ASSERT_TRUE(sresult[0] == 5 * 0xFFF);

    sresult = Int16x8(SSEMath::div<5>(UInt16x8(sresult)));
    printf ("Wsum10: %d\n", sresult[0]);
    ASSERT_TRUE(result == 0xFFF);

    printf ("SSE Blur: %d\n", sresult[0]);
}

TEST(VectorWrappers, testSignUnsign16)
{
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

    ASSERT_EQ(result[0], resultS);

}

#if 1
TEST(VectorWrappers, testShuffle)
{
    uint32_t data[4] = {3,2,1,0};
    Int32x4 dv0(data);
    Int32x4 dv1(data);

    static const uint shuffle0 = _MM_SHUFFLE(3,1,2,0);
    static const uint shuffle1 = _MM_SHUFFLE(2,0,3,1);

   
//    cout << "VEXT " << Int32x4(vextq_s32(dv0.data, dv0.data, 1)) << endl;

//    dv0 = Int32x4(vuzp1q_s32(dv0.data, vextq_s32(dv0.data, dv0.data, 1)));
//    dv1 = Int32x4(vuzp1q_s32(vextq_s32(dv1.data, dv1.data, 1), dv1.data));

    dv0.shuffle<shuffle0>();
    dv1.shuffle<shuffle1>();

    uint32_t expected0[4] = {3,1,2,0};
    uint32_t expected1[4] = {2,0,3,1};

    uint32_t out0[4];
    uint32_t out1[4];

    dv0.save(out0);
    dv1.save(out1);

    cout << "Testing shuffle0 " << shuffle0 << endl;
    cout << "Testing shuffle1 " << shuffle1 << endl;

    cout << "First shuffle:" << endl;

    for (unsigned i = 0; i < 4; i++)
    {
        cout << out0[i] << " " << expected0[i] << endl;
    }

    cout << "Second shuffle:" << endl;

    for (unsigned i = 0; i < 4; i++)
    {
        cout << out1[i] << " " << expected1[i] << endl;
    }

    for (unsigned i = 0; i < 4; i++)
    {
        CORE_ASSERT_TRUE_P(out0[i] == expected0[i], ("Shuffle eror:%i [%d != %d]", i, out0[i], expected0[i]));
        CORE_ASSERT_TRUE_P(out1[i] == expected1[i], ("Shuffle eror:%i [%d != %d]", i, out1[i], expected1[i]));
    }
}
#endif

TEST(VectorWrappers,testSaveLower)
{
    uint16_t indata[8] = {0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x00006, 0x0007, 0x0008};
    Int16x8 in(indata);

    uint64_t out;
    in.saveLower((uint16_t *)&out);

    uint64_t expected = 0x0004000300020001UL;

    printf("0x%08lX 0x%08lX\n", out, expected);
    CORE_ASSERT_TRUE_P(out == expected, ("Mask error [%ld != %ld]", out, expected));
}

TEST(VectorWrappers, testMask)
{
   uint32_t data[4] = {0xFFFFFFFFu, 0,0xFFFFFFFFu,0};
   Int32x4 a(data);
   uint16_t t = a.maskToInt2bit();
   // 0000111100001111
   uint16_t expected = 0x0F0F;

   CORE_ASSERT_TRUE_P(t == expected, ("Mask error [%d != %d]", t, expected));
}


TEST(VectorWrappers, testShiftLogical)
{

   uint16_t indata[8] = {0x2, 0x8, 0x20, 0x80, 0x200, 0x800, 0x2000, 0x8000};
   Int16x8 in(indata);
   Int16x8 out = in.shiftLogical(1);
   uint16_t data[8];
   out.save(data);

   uint16_t expected[8] = {0x1, 0x4, 0x10, 0x40, 0x100, 0x400, 0x1000, 0x4000};

   for (unsigned i = 0; i < 8; i++)
   {
       cout << out[i] << " " << expected[i] << endl;
   }

   for (unsigned i = 0; i < 8; i++)
   {
       CORE_ASSERT_TRUE_P(out[i] == expected[i], ("Shuffle eror:%i [%d != %d]", i, data[i], expected[i]));
   }
}

TEST(VectorWrappers, unpackLoHigh)
{
   uint16_t indata0[8] = {0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107, 0x108};
   uint16_t indata1[8] = {0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208};

   Int16x8 in0(indata0);
   Int16x8 in1(indata1);

   Int32x4 lower = Int16x8::unpackLower (in0, in1);
   Int32x4 upper = Int16x8::unpackHigher(in0, in1);

   uint32_t data0[4];
   uint32_t data1[4];

   lower.save(data0);
   upper.save(data1);

   uint32_t expected0[4] = {0x02010101, 0x02020102, 0x02030103, 0x02040104};
   uint32_t expected1[4] = {0x02050105, 0x02060106, 0x02070107, 0x02080108};

   for (unsigned i = 0; i < 4; i++)
   {
       printf("0x%08X 0x%08X\n", data0[i], expected0[i]);
   }

   printf("==============\n");
   for (unsigned i = 0; i < 4; i++)
   {
       printf("0x%08X 0x%08X\n", data1[i], expected1[i]);
   }

   for (unsigned i = 0; i < 4; i++)
   {
       CORE_ASSERT_TRUE_P(data0[i] == expected0[i], ("Unpack lower error:%i [%d != %d]", i, data0[i], expected0[i]));
       CORE_ASSERT_TRUE_P(data1[i] == expected1[i], ("Unpack upper error:%i [%d != %d]", i, data1[i], expected1[i]));
   }
}

TEST(VectorWrappers, unpackLoHigh32)
{
   uint32_t indata0[4] = {0x101, 0x102, 0x103, 0x104};
   uint32_t indata1[4] = {0x201, 0x202, 0x203, 0x204};

   Int32x4 in0(indata0);
   Int32x4 in1(indata1);

   Int64x2 lower = Int32x4::unpackLower (in0, in1);
   Int64x2 upper = Int32x4::unpackHigher(in0, in1);

   uint64_t data0[2];
   uint64_t data1[2];

   lower.save(data0);
   upper.save(data1);

   uint64_t expected0[2] = {0x0000020100000101UL, 0x0000020200000102UL};
   uint64_t expected1[2] = {0x0000020300000103UL, 0x0000020400000104UL};

   for (unsigned i = 0; i < 2; i++)
   {
       printf("0x%08X 0x%08X\n", data0[i], expected0[i]);
   }

   printf("==============\n");
   for (unsigned i = 0; i < 2; i++)
   {
       printf("0x%08X 0x%08X\n", data1[i], expected1[i]);
   }

   for (unsigned i = 0; i < 2; i++)
   {
       CORE_ASSERT_TRUE_P(data0[i] == expected0[i], ("Unpack lower error:%i [%" PRIu64 " != %" PRIu64 "]", i, data0[i], expected0[i]));
       CORE_ASSERT_TRUE_P(data1[i] == expected1[i], ("Unpack upper error:%i [%" PRIu64 " != %" PRIu64 "]", i, data1[i], expected1[i]));
   }
}



TEST(VectorWrappers, testSelector)
{
    Int16x8 con8bit((int16_t)0xFF);
    Int16x8 con0bit((int16_t)0x00);

    Int16x8 a(256, 257, 2, 340, 1, 23, 45, 625);
    std::cout << "input  " << a << std::endl;

    Int16x8 flags = a > con8bit;
    std::cout << "flags" << flags << std::endl;  


    Int16x8 ifTrue  = (flags & con8bit);
    Int16x8 ifFalse = andNot(flags, a);

    std::cout << "ifTrue" << ifTrue << std::endl;
    std::cout << "ifFalse" << ifFalse << std::endl;


    a = SSEMath::selector(flags, con8bit, a);
    std::cout << "cliptop" << a << std::endl;
    int16_t clipTopCheck[8] = {255, 255, 2, 255, 1, 23, 45, 255};
    int16_t topData[8];
    a.save(topData);

    for (int i = 0; i < 8; i++) {
        CORE_ASSERT_TRUE_P(clipTopCheck[i] == topData[i], ("Selector seem to have a problem at %d [%d %d]\n", i, clipTopCheck[i], topData[i]));
    }


    Int16x8 b(-256, 257, -2, 340, 1, 23, -45, 625);
    std::cout << "input  " << b << std::endl;
    b = SSEMath::selector(b < con0bit, con0bit, b);
    std::cout << "clipbottom" << b << std::endl;

    int16_t clipBottomCheck[8] = {0, 257, 0, 340, 1, 23, 0, 625};
    int16_t bottomData[8];
    b.save(bottomData);


    for (int i = 0; i < 8; i++) {
        CORE_ASSERT_TRUE_P(clipBottomCheck[i] == bottomData[i], ("Selector seem to have a problem at %d [%d %d]\n", i, clipBottomCheck[i], bottomData[i]));
    }




}


#endif // defined(WITH_SSE) || defined(WITH_NEON)
