/**
 * \file main_test_rgb24buffer.cpp
 * \brief This is the main file for the test rgb24buffer 
 *
 * \date Aug 10, 2011
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "rgb24Buffer.h"
#include "../../core/fileformats/bmpLoader.h"

using namespace std;
using namespace corecvs;

TEST(RGB24BufferTest, testDrawLine)
{
    RGB24Buffer *buffer = new RGB24Buffer(1080, 1920);
    buffer->drawLine(1689, 1066, -79, 387, RGBColor(255,255,255));
    delete buffer;
}

TEST(RGB24BufferTest, testConversionToG12)
{
    RGB24Buffer *buffer = new RGB24Buffer(100, 100);
    for (int i = 0; i < 10; i++) {
        buffer->drawLine(1 + i * 4, 10, 90, 90, RGBColor(255, 255 - 20 * i, 255));
    }

    G12Buffer *result = buffer->toG12Buffer();
    BMPLoader().save("rgb24.bmp", buffer);
    BMPLoader().save("g12.bmp",   result);

    delete_safe(result);
    delete_safe(buffer);
}
