/**
 * \file main_test_noise.cpp
 * \brief This is the main file for the test noise 
 *
 * \date июля 23, 2016
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "g12Buffer.h"
#include "g8Buffer.h"

#include "rgb24Buffer.h"
#include "bmpLoader.h"
#include "perlinNoise.h"


using namespace std;

TEST(noise, testnoise)
{
    int H = 360;
    int W = 360;


    G8Buffer    *noise  = new G8Buffer   (H, W);
    RGB24Buffer *noiser = new RGB24Buffer(H, W);

    G8Buffer    *turb  = new G8Buffer   (H, W);
    RGB24Buffer *turbr = new RGB24Buffer(H, W);

    SYNC_PRINT(("Initing...\n"));

    PerlinNoise gen;

    SYNC_PRINT(("Generating...\n"));

    for (int i = 0; i < noise->h; i++)
    {
        for (int j = 0; j < noise->w; j++)
        {
            Vector3dd p = Vector3dd(i,j,0) / 40.0;

            double sample = gen.noise(p);
            cout << sample << " " << endl;

            noise ->element(i,j) = sample * 255.0;
            noiser->element(i,j) = RGBColor::FromDouble(gen.noise3d(p) * 255.0);

            turb ->element(i,j) = gen.turbulence(p) * 255.0;
            turbr->element(i,j) = RGBColor::FromDouble(gen.turbulence3d(p) * 255.0);

        }
    }

    SYNC_PRINT(("Saving...\n"));

    BMPLoader().save("noise.bmp" , noise);
    BMPLoader().save("noiser.bmp", noiser);

    BMPLoader().save("turb.bmp" , turb);
    BMPLoader().save("turbr.bmp", turbr);

    SYNC_PRINT(("Cleanup...\n"));

    delete_safe(noise);
    delete_safe(noiser);
    delete_safe(turb);
    delete_safe(turbr);


}
