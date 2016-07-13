/**
 * \file main_test_buffer.cpp
 * \brief This is the main file for the test buffer
 *
 * \date Aug 21, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#ifndef TRACE
//#define TRACE
#endif

#include <iostream>
#include <type_traits>
#include <map>
#include "gtest/gtest.h"

#include "global.h"

#include "matrix33.h"
#include "bufferFactory.h"
#include "g12Buffer.h"
#include "projectiveTransform.h"
#include "ppmLoader.h"
#include "bmpLoader.h"
#include "integralBuffer.h"
#include "derivativeBuffer.h"
#include "g12Buffer.h"
#include "abstractBuffer.h"
#include "memoryBlock.h"
#include "matrix33.h"
#include "projectiveTransform.h"
#include "bufferFactory.h"
#include "rgb24Buffer.h"
#include "rgbColor.h"
#include "derivativeBuffer.h"
#include "memoryBlock.h"
#include "rgbColor.h"
#include "abstractPainter.h"
#include "g8Buffer.h"
#include "booleanBuffer.h"
#include "polynomial.h"

using namespace std;
using namespace corecvs;

TEST(Generic, DISABLED_testAlwaysFail)
{
    ASSERT_EQ(0, 1) << "Just Fail\n";
}

class TestAbstractBufferClass {
public:
    static int counter;
    static std::map<void*, int> advancedCounter;
    TestAbstractBufferClass()
    {
        counter++;
        advancedCounter[this]++;
    }
    ~TestAbstractBufferClass()
    {
        counter--;
        advancedCounter[this]--;
    }
    static bool checkCorrectness()
    {
        if (counter != 0)
        {
            std::cout << "Reference counter is not null" << std::endl;
            return false;
        }
        for (auto&p : advancedCounter)
            std::cout << p.first << " : " << p.second << std::endl;
        for (auto&p : advancedCounter)
            if (p.second != 0)
            {
                std::cout << "Reference counter for " << p.first << " is not null" << std::endl;
                return false;
            }
        return true;
    }
    int boo;
};

int TestAbstractBufferClass::counter = 0;
std::map<void*, int> TestAbstractBufferClass::advancedCounter;

TEST(Buffer, testBufferCC)
{
    int N = 5;
    AbstractBuffer<AbstractBuffer<int, int>, int> buff(N, N, AbstractBuffer<int, int>(N, N));

    int idx = 0;
    for (int i1 = 0; i1 < N; ++i1)
    {
        for (int j1 = 0; j1 < N; ++j1)
        {
            for (int i2 = 0; i2 < N; ++i2)
            {
                for (int j2 = 0; j2 < N; ++j2)
                {
                    buff.element(i1, j1).element(i2, j2) = idx++;
                }
            }
        }
    }

    int idx2 = 0;
    for (int i1 = 0; i1 < N; ++i1)
    {
        for (int j1 = 0; j1 < N; ++j1)
        {
            for (int i2 = 0; i2 < N; ++i2)
            {
                for (int j2 = 0; j2 < N; ++j2)
                {
                    ASSERT_EQ(buff.element(i1, j1).element(i2, j2), idx2++);
                }
            }
        }
    }

}

TEST(Buffer, testG12Buffer)
{
#if __GNUG__ && __GNUC__ < 5
    ASSERT_TRUE(__has_trivial_copy(int));
    ASSERT_TRUE(__has_trivial_copy(double));
    ASSERT_TRUE(__has_trivial_copy(RGBColor));
    ASSERT_TRUE(__has_trivial_copy(corecvs::Vector3dd));
#else
    ASSERT_TRUE(std::is_trivially_copy_constructible<int>::value);
    ASSERT_TRUE(std::is_trivially_copy_constructible<double>::value);
    ASSERT_TRUE(std::is_trivially_copy_constructible<RGBColor>::value);
    ASSERT_TRUE(std::is_trivially_copy_constructible<corecvs::Vector3dd>::value);
#endif
    ASSERT_TRUE(std::is_trivially_destructible<int>::value);
    ASSERT_TRUE(std::is_trivially_destructible<double>::value);
    ASSERT_TRUE(std::is_trivially_destructible<RGBColor>::value);
#if __GNUG__ && __GNUC__ < 5
    ASSERT_TRUE(has_trivial_default_constructor<int>());
    ASSERT_TRUE(has_trivial_default_constructor<double>());
#else
    ASSERT_TRUE(std::is_trivially_constructible<int>::value);
    ASSERT_TRUE(std::is_trivially_constructible<double>::value);
#endif

    ASSERT_FALSE(std::is_trivially_destructible<TestAbstractBufferClass>::value);
    ASSERT_FALSE(std::is_trivially_destructible<Polynomial>::value);
    /*
     * TODO: Check if we would (or would not) like this test to fail
     */
    //ASSERT_TRUE(std::is_trivially_constructible<RGBColor>::value);

   /* Test case 1: Create and destroy buffer*/
   G12Buffer *a = new G12Buffer(1,1);
   a->element(0,0) = 0;
   delete a;

   /* Test case 2: Test internal buffer destruction */
   TestAbstractBufferClass::counter = 0;
   TestAbstractBufferClass::advancedCounter.clear();

   AbstractBuffer<TestAbstractBufferClass> *b = new AbstractBuffer<TestAbstractBufferClass>
   ( AbstractBuffer<TestAbstractBufferClass>::DATA_ALIGN_GRANULARITY,
     AbstractBuffer<TestAbstractBufferClass>::DATA_ALIGN_GRANULARITY );
   delete b;
   std::cout << TestAbstractBufferClass::counter << std::endl;
   CORE_ASSERT_TRUE(TestAbstractBufferClass::counter == 0, "Abstract buffer violates the C++ new/delete contract")
    ASSERT_TRUE(TestAbstractBufferClass::checkCorrectness());

   /* Test case 3: Test internal buffer destruction */
   TestAbstractBufferClass::advancedCounter.clear();
   TestAbstractBufferClass::counter = 0;

   AbstractBuffer<TestAbstractBufferClass> *d = new AbstractBuffer<TestAbstractBufferClass>
   ( AbstractBuffer<TestAbstractBufferClass>::DATA_ALIGN_GRANULARITY + 1,
     AbstractBuffer<TestAbstractBufferClass>::DATA_ALIGN_GRANULARITY + 1);
   delete d;
   CORE_ASSERT_TRUE(TestAbstractBufferClass::counter == 0, "Abstract buffer violates the C++ new/delete contract")
    ASSERT_TRUE(TestAbstractBufferClass::checkCorrectness());


   /* Test case 4: Alignment */
   const unsigned testLen = 100;
   G12Buffer **c = new G12Buffer *[testLen];
   for (unsigned i = 0; i < testLen; i++)
   {
       c[i] = new G12Buffer(i+1,i+i);
       CORE_ASSERT_FALSE(((uintptr_t)c[i]->data) & AbstractBuffer<TestAbstractBufferClass>::DATA_ALIGN_GRANULARITY, "Buffer alignment broken");

       uintptr_t line1 = (uintptr_t)&(c[i]->element(0,0));
       uintptr_t line2 = (uintptr_t)&(c[i]->element(1,0));
       CORE_ASSERT_FALSE((line1 - line2) & AbstractBuffer<TestAbstractBufferClass>::DATA_ALIGN_GRANULARITY, "Stride alignment broken");
   }

   for (unsigned i = 0; i < testLen; i++)
   {
       delete c[i];
   }
   delete[] c;

    /* Test case 5: Views */
    TestAbstractBufferClass::advancedCounter.clear();
    TestAbstractBufferClass::counter = 0;
    AbstractBuffer<TestAbstractBufferClass> *main = new AbstractBuffer<TestAbstractBufferClass>(10, 10);
    int oldCnt = TestAbstractBufferClass::counter;
    ASSERT_EQ(oldCnt, 100);
    AbstractBuffer<TestAbstractBufferClass> *view1 = main->createViewPtr<AbstractBuffer<TestAbstractBufferClass>>();
    ASSERT_EQ(oldCnt, TestAbstractBufferClass::counter);
    delete view1;
    ASSERT_EQ(oldCnt, TestAbstractBufferClass::counter);
    AbstractBuffer<TestAbstractBufferClass> *view2 = main->createViewPtr<AbstractBuffer<TestAbstractBufferClass>>();
    delete main;
    ASSERT_EQ(oldCnt, TestAbstractBufferClass::counter);
    delete view2;
    ASSERT_EQ(0, TestAbstractBufferClass::counter);
    ASSERT_TRUE(TestAbstractBufferClass::checkCorrectness());
}

TEST(Buffer, testBilinearTransform)
{
    Matrix33 inverseLeftMatrix(
        1, 0, -13,
        0, 1, -9.5,
        0, 0, 1
    );

    ProjectiveTransform inverseLeft(inverseLeftMatrix);

    G12Buffer *image = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    CORE_ASSERT_TRUE(image, "Could not open test image\n");
    CORE_ASSERT_TRUE(image->verify(), "Input image is corrupted");
    G12Buffer *buffer1Transformed = image->doReverseTransform<ProjectiveTransform>(&inverseLeft, image->h, image->w);
    CORE_ASSERT_TRUE(buffer1Transformed->verify(), "Result image is corrupted");

    BMPLoader   *loader = new BMPLoader();
    RGB24Buffer *toSave = new RGB24Buffer(buffer1Transformed);
    loader->save("proc.bmp", toSave);

    delete_safe(toSave);
    delete_safe(loader);
    delete_safe(buffer1Transformed);
    delete_safe(image);
}

TEST(Buffer, testDerivative)
{
    uint16_t inputData[] = {
        1,   2,  3,  4,
        5,   6,  7,  8,
        9,  10, 11, 12,
        13, 14, 15, 16
    };

    G12Buffer *input = new G12Buffer(4, 4, inputData);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
        {
            std::cout << i << ", " << j << std::endl;
            ASSERT_EQ(input->element(i, j), inputData[i * 4 + j]);
        }

    DerivativeBuffer *result = new DerivativeBuffer(input);
    for (int i = 1; i < result->h - 1; i++)
    {
        for (int j = 1; j < result->w - 1; j++)
        {
            cout << "  " << result->element(i,j);
            CORE_ASSERT_TRUE(result->element(i, j) == Vector2d<int16_t>(8, 32), "Error computing buffer");
        }
        cout << "\n";
    }
    delete_safe(result);
    delete_safe(input);
}

TEST(Buffer, testNonMinimal)
{
    G12Buffer *image = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    CORE_ASSERT_TRUE(image, "Could not open test image\n");
    CORE_ASSERT_TRUE(image->verify(), "Input image is corrupted");

    DerivativeBuffer *result = new DerivativeBuffer(image);
    G12Buffer *filtered = result->nonMaximalSuppression();

    BMPLoader *loader = new BMPLoader();
    RGB24Buffer *toSave = new RGB24Buffer(filtered);
    loader->save("proc111.bmp", toSave);

    cout << "Saved result";
    delete_safe(toSave);
    delete_safe(loader);
    delete_safe(filtered);
    delete_safe(result);
    delete_safe(image);
}

TEST(Buffer, testBufferDifference)
{
    G12Buffer *input1 = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    G12Buffer *input2 = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0002_c0.pgm");

    G12Buffer *output1 = G12Buffer::difference(input1, input2);
    (BMPLoader()).save("difference.bmp", output1);

    G12Buffer *output2 = output1->binarize(500);
    (BMPLoader()).save("movemask.bmp", output2);

    delete_safe(output2);
    delete_safe(output1);
    delete_safe(input2);
    delete_safe(input1);
}

TEST(Buffer, testBufferThreshold)
{
    G12Buffer *input1 = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    int level = 1500;
    G12Buffer *output = input1->binarize(level);
    (BMPLoader()).save("threshold.bmp", output);
    delete_safe(output);
    delete_safe(input1);
}

TEST(Buffer, testMemoryBlock)
{
    MemoryBlockRef block;
    block.allocate(500, 0xFF);
    MemoryBlockRef block1(block);
    MemoryBlockRef block2;
    block2 = block1;
}

class TestDummyClass {
public:
    int i;
    int j;
};

TEST(Buffer, testObjectBlock)
{
    /*ObjectRef<int> obj;
    obj.allocate();
    *obj.get() = 18;*/

    ObjectRef<TestDummyClass> ptr;
    ptr.allocate();
    ptr.get()->i = 19;
    ptr.get()->j = 20;

    ObjectRef<TestDummyClass> ptr1 = ptr;
}

TEST(Buffer, testFont)
{
    RGB24Buffer *buffer = new RGB24Buffer(400, 1600);
    AbstractPainter<RGB24Buffer> painter(buffer);
    painter.drawFormat(5,  5, RGBColor(0xFFFFFF), 1, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    painter.drawFormat(5, 25, RGBColor(0xFF00FF), 1, "abcdefghijklmnopqrstuvwxyz");
    painter.drawFormat(5, 45, RGBColor(0xFFFF00), 1, "0123456789%?$:\'\"");

    const char *string = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRTUVWXYZ";
    for (unsigned i = 0; i < strlen(string); i++)
    {
        printf("[%c]\n", string[i]);
        painter.drawGlyph(i * 20, 55, string[i], RGBColor(0xFFFF00), 1);
    }

    const char *string1 = "!@#$%^&*(){}[]_+-\"<>\\:;.,`";
    for (unsigned i = 0; i < strlen(string1); i++)
    {
        printf("[%c]\n", string1[i]);
        painter.drawGlyph(i * 20, 85, string1[i], RGBColor(0xFFFF00), 1);
    }

    painter.drawFormatVector( 5, 110, RGBColor(0xFFFFFF), 1, string);
    painter.drawFormatVector( 5, 145, RGBColor(0xFF00FF), 1, string1);
    painter.drawFormatVector( 5, 200, RGBColor(0xFFFFFF), 2, string1);

    (BMPLoader()).save("font.bmp", buffer);
    delete_safe(buffer);
}

bool beforeLastBit(int i)
{
	int r = i;
	int k = 0;
	for (k = 0; k < 16; k++)
	{
		r >>= 1;
		if (r == 0) break;
	}
	return (i & (1 << (k - 1))) != 0;
}

int lastBit(int i)
{
	int r = i;
	int k = 0;
	for (k = 0; k < 16; k++)
	{
		r >>= 1;
		if (r == 0) break;
	}
	return k;
}

TEST(Buffer, _testReduceChessboard)
{
    RGB24Buffer *buffer = new RGB24Buffer(512, 512);
    for (int i = 0; i < buffer->h ; i++)
    {
        for (int j = 0; j < buffer->w ; j++)
        {
        	int last1 = lastBit(i);
        	int last2 = lastBit(j);

        	int max = std::max(last1, last2);
        	int selector1 = max - 2;
        	int selector2 = max - 2;
        	if (last1 == last2)
        	{
        		selector1 = last1 - 1;
				selector2 = last2 - 1;
        	}

        	bool isWhite = (!!(i & (1 << selector1))) ^  (!!(j & (1 << selector2)));
        	buffer->element(buffer->h - 1 - i, buffer->w - 1 - j) = isWhite ? RGBColor(0xFFFFFF) : RGBColor(0x000000);
        }
    }
    (BMPLoader()).save("chess.bmp", buffer);
    delete_safe(buffer);
}

TEST(Buffer, _testReduceChessboard1)
{
    RGB24Buffer *buffer = new RGB24Buffer(512, 512);
    for (int i = 0; i < buffer->h ; i++)
    {
        for (int j = 0; j < buffer->w ; j++)
        {
        	int last1 = lastBit(i);
        	int last2 = lastBit(j);

        	/*int max = std::max(last1, last2);
        	int selector1 = max - 2;
        	int selector2 = max - 2;
        	if (last1 == last2)
        	{
        		selector1 = last1 - 1;
				selector2 = last2 - 1;
        	}*/

        	int selector1 = last1 - 1;
        	int selector2 = last2 - 1;

        	bool isWhite = (!!(i & (1 << selector1))) ^  (!!(j & (1 << selector2)));
        	buffer->element(buffer->h - 1 - i, buffer->w - 1 - j) = isWhite ? RGBColor(0xFFFFFF) : RGBColor(0x000000);
        }
    }
    (BMPLoader()).save("chess.bmp", buffer);
    delete_safe(buffer);
}

TEST(Buffer, testBoolean)
{
    int h = 45;
    int w = 45;
    G8Buffer *buffer = new G8Buffer(h, w);

    AbstractPainter<G8Buffer> painter(buffer);
    painter.drawCircle(    w / 4,     h / 4, w / 4, 255 );
    painter.drawCircle(    w / 4, 3 * h / 4, w / 4, 255 );
    painter.drawCircle(3 * w / 4,     h / 4, w / 4, 255 );
    painter.drawCircle(3 * w / 4, 3 * h / 4, w / 4, 255 );

    RGB24Buffer *sourceImage = new RGB24Buffer(h, w);
    sourceImage->drawG8Buffer(buffer);
    (BMPLoader()).save("source.bmp", sourceImage);

    BooleanBuffer packedBuffer(buffer);
    G8Buffer *unpackBuffer = packedBuffer.unpack(0, 255);
    RGB24Buffer *image = new RGB24Buffer(unpackBuffer->h, unpackBuffer->w);
    image->drawG8Buffer(unpackBuffer);

    (BMPLoader()).save("unpacked.bmp", image);

    packedBuffer.printBuffer();

    delete_safe(unpackBuffer);
    delete_safe(image);
    delete_safe(sourceImage);
    delete_safe(buffer);
}
