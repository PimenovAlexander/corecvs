/**
 * \file main_test_integral.cpp
 * \brief This is the main file for the test integral
 *
 * \date Aug 22, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#ifdef WITH_SSE
#include <emmintrin.h>
#endif
#include <iostream>
#include <stdint.h>

#include "global.h"

#include "integralBuffer.h"
#include "bufferFactory.h"

using namespace corecvs;

void testIntergralBufferGeneration ()
{
    cout << "Testing the Integral Buffer generation" << endl;

    uint16_t i[] = {
            1,   2,  3,  4,
            5,   6,  7,  8,
            9,  10, 11, 12,
            13, 14, 15, 16
    };

    uint32_t r[] = {
             0,  0,   0,  0,  0,
             0,     1,   3,  6, 10,
             0,     6,  14, 24, 36,
             0,    15,  33, 54, 78,
             0,    28,  60, 96, 136
        };

    AbstractContiniousBuffer<uint16_t, int32_t> *iBuf =
        new AbstractContiniousBuffer<uint16_t, int32_t>(4, 4, i);

    AbstractContiniousBuffer<uint32_t, int32_t> *rBuf =
        new AbstractContiniousBuffer<uint32_t, int32_t>(5, 5, r);

    IntegralBuffer<uint32_t, uint16_t, int32_t> *integral =
        new IntegralBuffer<uint32_t, uint16_t, int32_t>(iBuf);

    ASSERT_TRUE(integral->isEqual(rBuf), "Buffers should be equal");

    delete iBuf;
    delete rBuf;
    delete integral;
}

void testIntergralBufferBlur ()
{
    uint16_t i[] = {
                1,   2,  3,  4,
                5,   6,  7,  8,
                9,  10, 11, 12,
                13, 14, 15, 16
        };

    uint16_t r[] = {
                 6, 7,
                10, 11
        };

    AbstractContiniousBuffer<uint16_t, int32_t> *iBuf =
            new AbstractContiniousBuffer<uint16_t, int32_t>(4, 4, i);
    AbstractContiniousBuffer<uint16_t, int32_t> *rBuf =
            new AbstractContiniousBuffer<uint16_t, int32_t>(2, 2, r);

    IntegralBuffer<uint32_t, uint16_t, int32_t> *integral =
        new IntegralBuffer<uint32_t, uint16_t, int32_t>(iBuf);

    AbstractBuffer<uint16_t, int32_t> *iBuf2;
    iBuf2 = integral->rectangularBlur<AbstractBuffer<uint16_t, int32_t> >(1,1);
    ASSERT_TRUE(iBuf2->isEqual(rBuf), "Buffers should be equal");

    delete iBuf;
    delete rBuf;
    delete iBuf2;
    delete integral;
}

void testIntegralBlurLarge()
{
    cout << "Testing bluring on large file" << endl;
    G12Buffer *input = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    G12IntegralBuffer *integral = new G12IntegralBuffer(input);

    unsigned h = integral->getEffectiveH();
    unsigned w = integral->getEffectiveW();
    unsigned* blur = new unsigned[h * w];

    const int BLUR_RADIUS = 2;

    G12Buffer *blurFast = integral->rectangularBlur<G12Buffer>(BLUR_RADIUS,BLUR_RADIUS);

    for (unsigned i = BLUR_RADIUS; i < h - BLUR_RADIUS; i++)
    {
        unsigned* offset = blur + i * w + BLUR_RADIUS;
        for (unsigned j = BLUR_RADIUS; j < w - BLUR_RADIUS; j++)
        {
            unsigned tmp = integral->rectangle(j - BLUR_RADIUS, i - BLUR_RADIUS, j + BLUR_RADIUS, i + BLUR_RADIUS );
            unsigned result = (tmp / ((2 * BLUR_RADIUS + 1) * (2 * BLUR_RADIUS + 1)));

            ASSERT_TRUE_P(blurFast->element(i - BLUR_RADIUS, j - BLUR_RADIUS) == result,
                   ("Error calculating blur buffer at %d %d  %d != %d",
                    i, j, blurFast->element(i - BLUR_RADIUS, j - BLUR_RADIUS), result));

            *offset = result;
            offset++;
        }
    }

    delete[] blur;
    delete blurFast;
    delete integral;
    delete input;

}

#ifdef WITH_SSE
ALIGN_STACK_SSE
void testIntergralBufferSSE (void)
{
    cout << "Testing the Integral Buffer SSE reading" << endl;


    uint16_t data[] = {
            1,   2,  3,  4,
            5,   6,  7,  8,
            9,  10, 11, 12,
            13, 14, 15, 16
    };

    G12Buffer *input = new G12Buffer(4,4,data);

//    uint32_t *a = new uint32_t;
    G12IntegralBuffer *integral = new G12IntegralBuffer(input);
//    delete a;

    __m128i result = integral->rectangle_sse(0,0,0,3);

    ALIGN_DATA(16) uint32_t resultArray[4] = {0,0,0,0};
    _mm_store_si128 ((__m128i *)resultArray, result);

    DOTRACE(("Results are: %u %u %u %u\n", resultArray[0],resultArray[1],resultArray[2], resultArray[3]));

    ASSERT_TRUE(resultArray[0] == 28, "Error in SSE integral buffer calculation 1");
    ASSERT_TRUE(resultArray[1] == 32, "Error in SSE integral buffer calculation 2");
    ASSERT_TRUE(resultArray[2] == 36, "Error in SSE integral buffer calculation 3");
    ASSERT_TRUE(resultArray[3] == 40, "Error in SSE integral buffer calculation 4");

    ASSERT_TRUE(integral->rectangle(0,0,0,3) == 28, "Error in integral buffer calculation 1");
    ASSERT_TRUE(integral->rectangle(1,0,1,3) == 32, "Error in integral buffer calculation 2");
    ASSERT_TRUE(integral->rectangle(2,0,2,3) == 36, "Error in integral buffer calculation 3");
    ASSERT_TRUE(integral->rectangle(3,0,3,3) == 40, "Error in integral buffer calculation 4");


    delete input;
    delete integral;
}

void testIntergralBufferSSE_New (void)
{
    cout << "Testing the Integral Buffer SSE reading New" << endl;


    uint16_t data[] = {
            1,   2,  3,  4,
            5,   6,  7,  8,
            9,  10, 11, 12,
            13, 14, 15, 16
    };

    G12Buffer *input = new G12Buffer(4,4,data);

//    uint32_t *a = new uint32_t;
    G12IntegralBuffer *integral = new G12IntegralBuffer(input);
//    delete a;

    Int32x4 result = integral->rectangle_sse_new(0,0,0,3);

    ALIGN_DATA(16) uint32_t resultArray[4] = {0,0,0,0};
    result.saveAligned(resultArray);


    DOTRACE(("New Results are: %u %u %u %u\n", resultArray[0],resultArray[1],resultArray[2], resultArray[3]));

    ASSERT_TRUE(resultArray[0] == 28, "Error in SSE integral buffer calculation 1");
    ASSERT_TRUE(resultArray[1] == 32, "Error in SSE integral buffer calculation 2");
    ASSERT_TRUE(resultArray[2] == 36, "Error in SSE integral buffer calculation 3");
    ASSERT_TRUE(resultArray[3] == 40, "Error in SSE integral buffer calculation 4");

    ASSERT_TRUE(integral->rectangle(0,0,0,3) == 28, "Error in integral buffer calculation 1");
    ASSERT_TRUE(integral->rectangle(1,0,1,3) == 32, "Error in integral buffer calculation 2");
    ASSERT_TRUE(integral->rectangle(2,0,2,3) == 36, "Error in integral buffer calculation 3");
    ASSERT_TRUE(integral->rectangle(3,0,3,3) == 40, "Error in integral buffer calculation 4");


    delete input;
    delete integral;
}


void testBlurBufferSSE (void)
{
    G12Buffer *image = new G12Buffer(14,15);
    for (int i = 0; i < image->h; i++)
    {
        for (int j = 0; j < image->w; j++)
        {
            image->element(i,j) = i * 5 + j;
        }
    }
    image->print();

    G12IntegralBuffer *integral = new G12IntegralBuffer(image);

    G12Buffer *blur = integral->rectangularBlur<G12Buffer>(2,2);
    G12Buffer *blurSSE = integral->rectangularBlurSSE(2,2);

    blur->print();
    printf("With SSE");
    blurSSE->print();


    delete blur;
    delete blurSSE;

    delete integral;
    delete image;
}
#endif



int main (int /*a*/, char * /*b*/[])
{
#ifdef WITH_SSE
    testBlurBufferSSE();
#endif

    testIntegralBlurLarge();
    testIntergralBufferBlur();
    testIntergralBufferGeneration ();

#ifdef WITH_SSE
    testIntergralBufferSSE();
    testIntergralBufferSSE_New();
#endif


    cout << "PASSED" << endl;
    return 0;
}
