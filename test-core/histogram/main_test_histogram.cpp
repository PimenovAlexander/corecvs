/**
 * \file main_test_histogram.cpp
 * \brief This is the main file for the test histogram 
 *
 * \date Feb 23, 2012
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/buffers/g12Buffer.h"
#include "core/buffers/histogram/histogram.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/bufferFactory.h"

using namespace corecvs;

TEST(Histogram, testBinarization)
{
    G12Buffer *image = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    if (image == nullptr)
    {
        cout << "Could not open test image" << endl;
        return;
    }
    CORE_ASSERT_TRUE(image != NULL, "missed testBinarization input data");

    Histogram *histogram = new Histogram(image);

    int meanLevel   = histogram->getMeanThreshold();
    int medianLevel = histogram->getMedianThreshold();
    int otsuLevel   = histogram->getOtsuThreshold();

    G12Buffer *meanImage   = image->binarize(meanLevel);
    BMPLoader().save("mean.bmp", meanImage);

    G12Buffer *medianImage = image->binarize(medianLevel);
    BMPLoader().save("median.bmp", medianImage);

    G12Buffer *otsuImage   = image->binarize(otsuLevel);
    BMPLoader().save("otsu.bmp", otsuImage);

    delete otsuImage;
    delete medianImage;
    delete meanImage;
    delete histogram;
    delete image;
}

TEST(Histogram, testAdd)
{
    Histogram A(0,10);
    Histogram B(5,15);

    for (int i=0; i <=10; i++)
        A.inc(i);


    for (int i=5; i <=15; i++) {
        B.inc(i);
        B.inc(i);
    }

    A.inc(6);
    B.inc(12);

    A.add(&B);

    // cout << A;

    ASSERT_TRUE(A.getArgumentMin() ==  0);
    ASSERT_TRUE(A.getArgumentMax() == 15);


    ASSERT_TRUE(A.getData(0)  == 1);
    ASSERT_TRUE(A.getData(1)  == 1);
    ASSERT_TRUE(A.getData(2)  == 1);
    ASSERT_TRUE(A.getData(3)  == 1);
    ASSERT_TRUE(A.getData(4)  == 1); // 5
    ASSERT_TRUE(A.getData(5)  == 3); // 8
    ASSERT_TRUE(A.getData(6)  == 4); // 12
    ASSERT_TRUE(A.getData(7)  == 3); // 15
    ASSERT_TRUE(A.getData(8)  == 3); // 18
    ASSERT_TRUE(A.getData(9)  == 3); // 21
    ASSERT_TRUE(A.getData(10) == 3); // 24
    ASSERT_TRUE(A.getData(11) == 2);
    ASSERT_TRUE(A.getData(12) == 3);
    ASSERT_TRUE(A.getData(13) == 2);
    ASSERT_TRUE(A.getData(14) == 2);
    ASSERT_TRUE(A.getData(15) == 2);


    ASSERT_EQ(A.getIntervalSum(-2, 10), 24);

}
