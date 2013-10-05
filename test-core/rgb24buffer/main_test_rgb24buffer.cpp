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
#include "global.h"
#include "rgb24Buffer.h"


using namespace std;
using namespace corecvs;

void testDrawLine( void )
{
    RGB24Buffer *buffer = new RGB24Buffer(1080, 1920);
    buffer->drawLine(1689, 1066, -79, 387, RGBColor(255,255,255));
    delete buffer;
}

int main (int /*argC*/, char ** /*argV*/)
{

    testDrawLine();

    cout << "PASSED" << endl;
    return 0;
}
