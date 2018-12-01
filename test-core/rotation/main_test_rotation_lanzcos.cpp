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
#include <core/utils/preciseTimer.h>
#include <core/stats/calculationStats.h>
#include <core/geometry/raytrace/perlinNoise.h>

using namespace corecvs;

RGB24Buffer *getNoiseImage(int h, int w)
{
    RGB24Buffer *turbr = new RGB24Buffer(h, w);

    SYNC_PRINT(("Initing...\n"));

    PerlinNoise gen;

    SYNC_PRINT(("Generating...\n"));

    for (int i = 0; i < turbr->h; i++)
    {
        for (int j = 0; j < turbr->w; j++)
        {
            Vector3dd p = Vector3dd(i,j,0) / 40.0;
            turbr->element(i,j) = RGBColor::FromDouble(gen.turbulence3d(p) * 255.0);
        }
    }
    return turbr;
}

TEST(RotationTest, testRotate)
{
    // RGB24Buffer *buffer = BufferFactory::getInstance()->loadRGB24Bitmap("hero.bmp");
    Statistics stats;

    stats.startInterval();
    RGB24Buffer *buffer = getNoiseImage(768, 1024);
    stats.resetInterval("Generating image");
    RGB24Buffer *rotated = RotateHelper::rotateWithLancoz(degToRad(15), buffer, 1006, 1187);
    stats.resetInterval("Rotating image");
    BMPLoader().save("rotated_15.bmp", rotated);
    stats.endInterval("Saving image");

    delete_safe(rotated);
    delete_safe(buffer);

    BaseTimeStatisticsCollector(stats).printAdvanced();
}

TEST(RotationTest, quality)
{

    Statistics stats;

    stats.startInterval();
    RGB24Buffer *buffer = getNoiseImage(768, 1024);
    stats.resetInterval("Generating image");
    RGB24Buffer *rotated   = RotateHelper::rotateWithLancoz(degToRad(15), buffer, 1006, 1187, 2);
    stats.resetInterval("Rotating image");
    RGB24Buffer *rotatedback = RotateHelper::rotateWithLancoz(degToRad(-15), rotated, 768, 1024, 2);
    stats.resetInterval("Rotating back");

    int value = 0;
    /*Lets cut the boundary to make comparison only inside */
    RGB24Buffer in  = buffer     ->createView<RGB24Buffer>(2,2, buffer->h - 4, buffer->w - 4);
    RGB24Buffer out = rotatedback->createView<RGB24Buffer>(2,2, buffer->h - 4, buffer->w - 4);


    RGB24Buffer *diff = RGB24Buffer::diff(&in, &out, &value);
    diff->mapOperationElementwize([](const RGBColor &in){return RGBColor::FromFloat(in.toFloat() * 200.0);});

    BMPLoader().save("rotated_diff.bmp", diff);

    cout << "Error value:" << value << endl;
    cout << "Per pixel  :" << (double) value / in.numElements() << endl;
    //cout << "Steps      :" << (double) value / in.numElements() * 256.0 << endl;

    delete_safe(diff);
    delete_safe(rotatedback);
    delete_safe(rotated);
    delete_safe(buffer);

    BaseTimeStatisticsCollector(stats).printAdvanced();
}

#if 0
TEST(RotationTest, testRotateMinus15angle)
{
    RGB24Buffer *buffer = BufferFactory::getInstance()->loadRGB24Bitmap("hero.bmp");
    BMPLoader().save("rotated_image_hero_minus_15.bmp", rotate_with_lanczoc_filter(-15, buffer));
    delete buffer;
}
#endif
