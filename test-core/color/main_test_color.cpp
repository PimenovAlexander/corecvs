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

#include "global.h"

#include "rgb24Buffer.h"



using namespace std;
using namespace corecvs;

void testRGB(void )
{
    ASSERT_TRUE(sizeof(RGBColor) == sizeof(uint32_t), "Compiler aligned RGB color in an unexpeced way");

    RGBColor black (  0,   0,   0);
    RGBColor gray  (128, 128, 128);
    RGBColor red   (255,   0,   0);
    RGBColor green (  0, 255,   0);
    RGBColor blue  (  0,   0, 255);
    RGBColor cyan  (  0, 255, 255);



    ASSERT_TRUE_P( black.hue() ==   0, ("Black %d %d\n", black.hue(),   0));
    ASSERT_TRUE_P(  gray.hue() ==   0, ("Gray  %d %d\n", gray.hue(),    0));
    ASSERT_TRUE_P(   red.hue() ==   0, ("Red   %d %d\n", red.hue(),     0));
    ASSERT_TRUE_P( green.hue() == 120, ("Green %d %d\n", green.hue(), 120));
    ASSERT_TRUE_P(  blue.hue() == 240, ("Blue  %d %d\n", blue.hue(),  240));
    ASSERT_TRUE_P(  cyan.hue() == 180, ("Cyan  %d %d\n", cyan.hue(),  180));


}

int main (int /*argC*/, char ** /*argV*/)
{
    testRGB();
        cout << "PASSED" << endl;
        return 0;
}
