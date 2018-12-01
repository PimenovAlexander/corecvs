/**
 * \file main_test_rotation_lanzcos.cpp
 * \brief This is the main file for the test rotation
 *
 * \date Nov 30, 2018
 * \author VladimirMaltsev
 *
 * \ingroup autotest
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "core/buffers/abstractBuffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "../../core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/bufferFactory.h"
#include "core/math/rotate.h"
#include <ctime>
#include <core/utils/preciseTimer.h>

using namespace corecvs;

TEST(RotationTest, testRotate15angle)
{
    RGB24Buffer *buffer = BufferFactory::getInstance()->loadRGB24Bitmap("hero.bmp");
    BMPLoader().save("rotated_image_hero_15.bmp", rotate_with_lanczoc_filter(15, buffer));
    delete buffer;
}

TEST(RotationTest, testRotateMinus15angle)
{
    RGB24Buffer *buffer = BufferFactory::getInstance()->loadRGB24Bitmap("hero.bmp");
    BMPLoader().save("rotated_image_hero_minus_15.bmp", rotate_with_lanczoc_filter(-15, buffer));
    delete buffer;
}



