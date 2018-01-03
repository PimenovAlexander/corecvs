/**
 * \file main_test_wuRasterizer.cpp
 * \brief This is the main file for the test wuRasterizer 
 *
 * \date Jan 02, 2018
 * \author vschellb
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "core/buffers/abstractBuffer.h"

using namespace corecvs;

TEST(wuRasterizer, testwuRasterizer) {
    AbstractBuffer<float> buffer1(100, 100, true);
    WuRasterizer::drawLine(buffer1, 0, 10, 10, 10, 2.);
    ASSERT_EQ(buffer1.element(0,10), 1.0);
    ASSERT_EQ(buffer1.element(0,11), 0.0);
    ASSERT_EQ(buffer1.element(10,10), 1.0);
    ASSERT_EQ(buffer1.element(10,11), 0.0);

    AbstractBuffer<float> buffer2(100, 100, true);
    WuRasterizer::drawLine(buffer2, 0, 0, 10, 10, 2.);
    ASSERT_EQ(buffer2.element(0,0), 1.0);
    ASSERT_EQ(buffer2.element(0,1), 0.0);
    ASSERT_EQ(buffer2.element(10,9), 0.0);
    ASSERT_EQ(buffer2.element(10,10), 1.0);
    ASSERT_EQ(buffer2.element(10,11), 0.0);
}
