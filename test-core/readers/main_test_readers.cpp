/**
 * \file main_test_readers.cpp
 * \brief This is the main file for the test readers 
 *
 * \date авг. 25, 2015
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"

#include "readers.h"
#include "rgbColor.h"

using namespace corecvs;

#ifdef WITH_SSE
TEST(Readers, testRGBReaders)  // TODO: add some checks...
{
    std::cout << "testRGBReaders()" << std::endl;

    RGBColor data[16] = { RGBColor::Yellow(), RGBColor::Indigo(), RGBColor::Violet(), RGBColor::Cyan() };

    FixedVector<Int32x4, 2> color = SSEReader2BBBB_QQQQ::read((uint8_t *)data);

    std::cout << color[0][0] << std::endl;
    std::cout << color[0][1] << std::endl;
    std::cout << color[0][2] << std::endl;

    std::cout << color[1][0] << std::endl;
    std::cout << color[1][1] << std::endl;
    std::cout << color[1][2] << std::endl;

    std::cout << data[0] << " " << data[1] << std::endl;
}
#endif
