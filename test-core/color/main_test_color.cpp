/**
 * \file main_test_color.cpp
 * \brief This is the main file for the test color 
 *
 * \date Apr 08, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */
#include <iostream>
#include <core/buffers/bufferFactory.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/rgbColor.h"

using namespace corecvs;

TEST(Color, testRGB)
{
    //cout << "sizeof(RGBColor) = " << sizeof(RGBColor) << endl;

    CORE_ASSERT_TRUE(sizeof(RGBColor) == sizeof(uint32_t), "Compiler aligned RGB color in an unexpeced way");

    RGBColor black (  0,   0,   0);
    RGBColor gray  (128, 128, 128);
    RGBColor red   (255,   0,   0);
    RGBColor green (  0, 255,   0);
    RGBColor blue  (  0,   0, 255);
    RGBColor cyan  (  0, 255, 255);

    CORE_ASSERT_TRUE_P(black.hue() ==   0, ("Black %d %d\n", black.hue(),   0));
    CORE_ASSERT_TRUE_P(gray.hue()  ==   0, ("Gray  %d %d\n", gray.hue(),    0));
    CORE_ASSERT_TRUE_P(red.hue()   ==   0, ("Red   %d %d\n", red.hue(),     0));
    CORE_ASSERT_TRUE_P(green.hue() == 120, ("Green %d %d\n", green.hue(), 120));
    CORE_ASSERT_TRUE_P(blue.hue()  == 240, ("Blue  %d %d\n", blue.hue(),  240));
    CORE_ASSERT_TRUE_P(cyan.hue()  == 180, ("Cyan  %d %d\n", cyan.hue(),  180));
}

TEST(Color, testRGBMulFloat)
{
    RGBColor white = RGBColor::White();

    RGBColor color = white * 0.5;
    cout << white << endl;
    cout << color << endl;
    ASSERT_TRUE(color.notTooFar(RGBColor::Gray(255*0.5)));
}

TEST(Color, testHSVCoversion)
{
    RGBColor testColors[] = {
        RGBColor::Black(), RGBColor::Red(), RGBColor::Green(), RGBColor::Blue(), RGBColor::Cyan(),
        RGBColor::Indigo(), RGBColor::Violet()
    };

    for (unsigned i = 0; i < CORE_COUNT_OF(testColors); i++)
    {
        cout << "Testing color: " << testColors[i] << std::endl;
        int h = testColors[i].hue();
        int s = testColors[i].saturation();
        int v = testColors[i].value();
        RGBColor rebuild = RGBColor::FromHSV(h,s,v);
        cout << "  HSV: " << h << "," << s  << "," << v << std::endl;
        cout << "  Res: " << rebuild << std::endl;
    }
}

TEST(Color, testHSVColorCircle)
{
    static const int SIZE = 201;

    std::unique_ptr<RGB24Buffer> buffer(new RGB24Buffer(2 * SIZE, SIZE * 3));

    for (uint i = 0; i < SIZE; i++)
    {
        for (uint j = 0; j < SIZE; j++)
        {
            Vector2dd d = Vector2dd(i, j) - Vector2dd(SIZE / 2.0, SIZE / 2.0);
            uint16_t h = ((int)radToDeg(d.argument()) + 360) % 360;
            uint16_t l = d.l2Metric() / (SIZE / 2) * 255.0;
            if (l > 255) {
                continue;
            }

            buffer->element(i, j          ) = RGBColor::FromHSV(h,      l, 128);
            buffer->element(i, j +    SIZE) = RGBColor::FromHSV(h,    128,   l);
            buffer->element(i, j + 2 *SIZE) = RGBColor::FromHSV(h, l / 2 ,   l / 2);

            buffer->element(i + SIZE, j          ) = RGBColor::FromHSVKitti(h,      l, 128);
            buffer->element(i + SIZE, j +    SIZE) = RGBColor::FromHSVKitti(h,    128,   l);
            buffer->element(i + SIZE, j + 2 *SIZE) = RGBColor::FromHSVKitti(h, l / 2 ,   l / 2);


        }
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(*buffer, "hsv.bmp");

}
