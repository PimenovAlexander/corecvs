/**
 * \file main_test_convert.cpp
 * \brief This is the main file for the test convert 
 *
 * \date февр. 25, 2015
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "core/utils/global.h"

#include "core/math/sse/sseWrapper.h"

using namespace std;
using namespace corecvs;


void testConvert1 (void)
{
    cout << "testConvert1 (void)" << endl;
#ifdef WITH_SSE
    const int len = 128 / sizeof(uint16_t);
    uint16_t input[len];

    for (int i = 0; i < len; i++) {
        input[i] = i;
    }

    Int16x8 A(input);
    cout << "Input" << A << endl;
    Int32x8 unpaked = A.expand();
    cout << "Output" << endl;
    cout << unpaked[0] << endl;
    cout << unpaked[1] << endl;
#endif

}

void testConvert2 (void)
{
    cout << "testConvert1 (void)" << endl;
#ifdef WITH_SSE
    const int len = 128 / sizeof(uint8_t);
    uint8_t input[len];

    for (int i = 0; i < len; i++) {
        input[i] = i;
    }

    Int8x16 In((uint8_t *)input);
    cout << "Input ";
    In.hexDump(cout);
    cout << endl;

    Int16x8 I1 = Int8x16::unpackLower (In, Int8x16((uint8_t)0x0));
    Int16x8 I2 = Int8x16::unpackHigher(In, Int8x16((uint8_t)0x0));

    cout << "Temp " << endl;
    I1.hexDump(cout); cout << endl;
    I2.hexDump(cout); cout << endl;

    Int32x8 A1 = I1.expand();
    Int32x8 A2 = I2.expand();

    cout << "result " << endl;
    A1[0].hexDump(cout); cout << endl;
    A1[1].hexDump(cout); cout << endl;
    A2[0].hexDump(cout); cout << endl;
    A2[1].hexDump(cout); cout << endl;


/*    unpaked[0] = 12;
    unpaked[0].hexDump(cout); cout << endl;

    Int32x8 c1 = Int16x8(unpaked[0].data).expand();
    cout << "C1 " << endl;
    c1[0].hexDump(cout); cout << endl;
    c1[1].hexDump(cout); cout << endl;*/


//    B = A & Int16x8(0xFF);
//    C = A

    /*Int32x8 unpaked = A.expand();
    cout << "Output" << endl;
    cout << unpaked[0] << endl;
    cout << unpaked[1] << endl;
    cout << unpaked[2] << endl;
    cout << unpaked[3] << endl;*/

#endif

}


int main (int /*argC*/, char **/*argV*/)
{
    testConvert1();
    testConvert2();
    cout << "PASSED" << endl;
    return 0;
}
