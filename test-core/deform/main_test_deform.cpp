/**
 * \file main_test_deform.cpp
 * \brief This is the main file for the test deform
 *
 * \date Mar 01, 2012
 * \author alexander
 *
 * \ingroup autotest
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/buffers/fixeddisp/fixedPointDisplace.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/projectiveTransform.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/alignment/radialCorrection.h"
#include "core/buffers/displacementBuffer.h"

#include "core/utils/preciseTimer.h"

using corecvs::Matrix33;
using corecvs::ProjectiveTransform;
using corecvs::G12Buffer;
using corecvs::BufferFactory;
using corecvs::FixedPointDisplace;
using corecvs::BMPLoader;
using corecvs::RGB24Buffer;

TEST(Deform, testFastDeform)
{
    Matrix33 inverseLeftMatrix(
        1, 0, -13,
        0, 1, -9.5,
        0, 0, 1
    );

    ProjectiveTransform inverseLeft(inverseLeftMatrix);

    G12Buffer *image = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");
    CORE_ASSERT_TRUE(image, "Could not open test image\n");
    CORE_ASSERT_TRUE(image->verify(), "Input image is corrupted");
    G12Buffer *buffer1Transformed = image->doReverseTransform<ProjectiveTransform>(&inverseLeft, image->h, image->w);
    CORE_ASSERT_TRUE(buffer1Transformed->verify(), "Result image is corrupted");

    FixedPointDisplace *displace = new FixedPointDisplace (inverseLeft, image->h, image->w);
    G12Buffer *buffer2Transformed = image->doReverseDeformationBlPrecomp(displace, image->h, image->w);

    BMPLoader().save("proc.bmp",  buffer1Transformed);
    BMPLoader().save("proc1.bmp", buffer2Transformed);

    G12Buffer *difference = G12Buffer::difference(buffer1Transformed, buffer2Transformed);
    difference->gainOffset(1/20.0, 0);

    BMPLoader().save("diff50.bmp", difference);

    delete difference;
    delete buffer2Transformed;
    delete buffer1Transformed;
    delete displace;
    delete image;
}

template<typename operation>
    void touchOperationElementwize (RGB24Buffer *buffer, operation &map)
    {
        for (int i = 0; i < buffer->h; i++)
        {
            RGBColor *thisElemRunner = &(buffer->element(i,0));
            for (int j = 0; j < buffer->w; j++)
            {
                map(i, j, (RGBColor *)thisElemRunner);
                thisElemRunner++;
            }
        }
    }

TEST(Deform, DISABLED_testBilinear)
{
    RGB24Buffer *image = new RGB24Buffer(2, 2);
    image->element(0,0) = RGBColor::Yellow();
    image->element(0,1) = RGBColor::Green();
    image->element(1,0) = RGBColor::Indigo();
    image->element(1,1) = RGBColor::Black();

    for (double i = 0; i < 1.0; i += 0.07 )
    {
        for (double j = 0; j < 1.0; j += 1.17 )
        {
            RGBColor d =  image->elementBlDouble(i, j);
            RGBColor f =  image->elementBlFixed (i, j);
            RGBColor s =  image->elementBlSSE   (i, j);

            RGBColor diff1 = RGBColor::diff(d,f);
            cout << d << " = " << f << " = " << s << " -> " <<  diff1 << endl;
        }
    }
    delete image;
}


TEST(Deform, DISABLED_testFastDeform24)
{
    cout << "Starting test: testFastDeform24 ()" << endl;
    Matrix33 inverseLeftMatrix(
        1, 0, -13,
        0, 1, -9.5,
        0, 0, 1
    );

    ProjectiveTransform inverseLeft(inverseLeftMatrix);


    RGB24Buffer *image = new RGB24Buffer(300, 300);

    auto operation = [](int i, int j, RGBColor *pixel)
    {
        i = i / 20;
        j = j / 20;
        if ( (i % 2) &&  (j % 2))   *pixel = RGBColor::Green();
        if (!(i % 2) &&  (j % 2))   *pixel = RGBColor::Yellow();
        if ( (i % 2) && !(j % 2))   *pixel = RGBColor::Red();
        if (!(i % 2) && !(j % 2))   *pixel = RGBColor::Blue();
    };
    touchOperationElementwize(image, operation);
    RGB24Buffer *buffer1Transformed = image->doReverseDeformation  <RGB24Buffer, ProjectiveTransform>(inverseLeft);

    /* This would produce empty buffer due to overflow. This is by design */
    RGB24Buffer *buffer2Transformed = image->doReverseDeformationBl<RGB24Buffer, ProjectiveTransform>(&inverseLeft);
    RGB24Buffer *buffer3Transformed = image->doReverseDeformationBlTyped<ProjectiveTransform>(&inverseLeft);

    FixedPointDisplace *displace = new FixedPointDisplace (inverseLeft, image->h, image->w);
    RGB24Buffer *buffer4Transformed = image->doReverseDeformationBlPrecomp(displace, image->h, image->w);


    BMPLoader().save("input.bmp", image);
    BMPLoader().save("transform-int.bmp"    , buffer1Transformed);
    BMPLoader().save("transform-bl.bmp"     , buffer2Transformed);
    BMPLoader().save("transform-blt.bmp"    , buffer3Transformed);
    BMPLoader().save("transform-precomp.bmp", buffer4Transformed);


    delete_safe(buffer1Transformed);
    delete_safe(buffer2Transformed);
    delete_safe(buffer3Transformed);
    delete_safe(buffer4Transformed);
    delete_safe(image);
}


void testRadialApplication(int scale);
void testRadialInversion(int scale);


TEST(Deform, testRadialApplication)  // TODO: move to perf-tests!
{
    testRadialApplication(1);
}

TEST(Deform, testRadialInversion)
{
    testRadialInversion(1);
}

TEST(Deform, testRectangularImage)
{
    LensDistortionModelParameters deformator;
    deformator.setPrincipalX(1296);
    deformator.setPrincipalY( 972);

    deformator.setTangentialX(0.000);
    deformator.setTangentialY(0.000);

    deformator.setAspect(1.0);
    deformator.setScale(1.0);
    deformator.setNormalizingFocal(1620.0);

    deformator.mKoeff.push_back(1.0);

    RadialCorrection T(deformator);

    Vector2dd min;
    Vector2dd max;

    T.getCircumscribedImageRect(0, 0, 1296 * 2, 972 * 2, min, max);
    cout << min  << " => " << max << endl;

    T.getInscribedImageRect    (0, 0, 1296 * 2, 972 * 2, min, max);
    cout << min  << " => " << max << endl;
}
