/**
 * \file main_test_sphericdist.cpp
 * \brief This is the main file for the test sphericdist 
 *
 * \date Feb 23, 2011
 * \author alexander
 *
 */

#include <iostream>
#include <stdio.h>
#include "gtest/gtest.h"

#include "global.h"
#include "g12Buffer.h"
#include "bmpLoader.h"
#include "bufferFactory.h"
#include "sphericalCorrectionLUT.h"


using namespace std;
using namespace corecvs;

#define LUT_LEN 48
double UnwarpToWarpLUT[LUT_LEN][2] = {
        {0.000000,1.000000},
        {92.018103,1.000147},
        {368.858400,1.000569},
        {832.893058,1.001204},
        {1488.123721,1.001957},
        {2340.255759,1.002706},
        {3396.804346,1.003313},
        {4667.236628,1.003634},
        {6163.156970,1.003532},
        {7898.535737,1.002884},
        {9889.998848,1.001588},
        {12157.173512,0.999565},
        {14723.126226,0.996758},
        {17614.882422,0.993135},
        {20864.074914,0.988676},
        {24507.728208,0.983378},
        {28589.232227,0.977243},
        {33159.537140,0.970276},
        {38278.636852,0.962480},
        {44017.424691,0.953855},
        {50460.014640,0.944390},
        {57706.684554,0.934063},
        {65877.620356,0.922838},
        {75117.732344,0.910663},
        {85602.896456,0.897471},
        {97548.121614,0.883176},
        {111218.379210,0.867677},
        {126943.072408,0.850861},
        {145135.700162,0.832603},
        {166320.863583,0.812773},
        {191172.008376,0.791241},
        {220565.004568,0.767879},
        {255655.686680,0.742568},
        {297994.392724,0.715199},
        {349699.190619,0.685674},
        {413724.785808,0.653907},
        {494292.693350,0.619823},
        {597603.478294,0.583355},
        {733063.993123,0.544441},
        {915503.912793,0.503025},
        {1169409.861614,0.459052},
        {1537585.847499,0.412466},
        {2100441.484776,0.363213},
        {3023919.418479,0.311237},
        {4697413.174758,0.256483},
        {8223083.381415,0.198898},
        {17832297.206513,0.138434},
        {63593741.644753,0.075052} };

TEST(Sphericdist, DISABLED_testUndistored)
{
    G12Buffer *input = BufferFactory::getInstance()->loadG12Bitmap("data/distored.pgm");

    vector<Vector2dd> lut;
    for (unsigned i = 0; i < LUT_LEN; i++)
    {
        lut.push_back(Vector2dd(UnwarpToWarpLUT[i][0] / 2.0, UnwarpToWarpLUT[i][1]));
    }

    RadiusCorrectionLUT radiusLUT(&lut);
    for (int i = 0; i < 400; i++)
    {
        printf("%lf %lf\n", (double)i, (double)radiusLUT.transformRadiusSquare(i * i));
    }

    Vector2dd center(input->w / 2.0, input->h / 2.0);
    SphericalCorrectionLUT corrector(center, &radiusLUT);

    G12Buffer *output = input->doReverseDeformationBl<G12Buffer, SphericalCorrectionLUT>(&corrector,
            input->h, input->w);

    BMPLoader().save("out.bmp", output);
}


//int main (int /*argC*/, char ** /*argV*/)
//{
//    testUndistored();
//        cout << "PASSED" << endl;
//        return 0;
//}

