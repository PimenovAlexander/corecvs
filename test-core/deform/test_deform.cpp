#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "rgb24Buffer.h"
#include "radialCorrection.h"
#include "preciseTimer.h"
#include "displacementBuffer.h"
#include "bmpLoader.h"

using std::flush;

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


void testRadialApplication(int scale)
{
    cout << "Starting test: testRadialApplication ()" << endl;
    RGB24Buffer *image = new RGB24Buffer(250 * scale, 400 * scale);

    auto operation = [](int i, int j, RGBColor *pixel)
    {
        i = i / 100;
        j = j / 200;
        if ( (i % 2) &&  (j % 2))   *pixel = RGBColor::Green();
        if (!(i % 2) &&  (j % 2))   *pixel = RGBColor::Yellow();
        if ( (i % 2) && !(j % 2))   *pixel = RGBColor::Red();
        if (!(i % 2) && !(j % 2))   *pixel = RGBColor::Blue();
    };
    touchOperationElementwize(image, operation);
    LensDistortionModelParameters deformator;
    deformator.setPrincipalX(image->w / 2);
    deformator.setPrincipalY(image->h / 2);

    deformator.setTangentialX(0.000001);
    deformator.setTangentialY(0.000001);

    deformator.setAspect(1.0);
    deformator.setScale(1.0);

    deformator.mKoeff.push_back( 0.0001);
    deformator.mKoeff.push_back(-0.00000002);
    deformator.mKoeff.push_back( 0.00000000000003);

    RadialCorrection T(deformator);
    PreciseTimer timer;

    /**
     * 1. Compute reverse image
     *
     *  Radial coorection stores transformation from real image to ideal.
     *  However to transform buffer we need to find inverse image of the pixel
     *
     *  We can either compute the inverse with the "analytical method" -
     *  the example is in testRadialInversion()
     *
     *  Or cache the inverse like we are doing here
     *
     **/
    cout << "Starting deformation inversion... " << flush;

    timer = PreciseTimer::currentTime();
    DisplacementBuffer *inverse = DisplacementBuffer::CacheInverse(&T,
        image->h, image->w,
        0.0,0.0,
        (double)image->w, (double)image->h, 0.5
    );
    cout << "done in: " << timer.usecsToNow() << "us" << endl;

    cout << "Applying deformation inversion... " << flush;
    timer = PreciseTimer::currentTime();
    RGB24Buffer *deformed = image->doReverseDeformationBlTyped<DisplacementBuffer>(inverse);
    cout << "done in: " << timer.usecsToNow() << "us" << endl;

    /**
     * 2. We have 4 ways to invert the transform.
     *   1. Apply T directly
     *   2. Create distortion buffer that will cache the invert
     *
     **/

    /*2.1*/
    cout << "Applying forward deformation... " << flush;
    timer = PreciseTimer::currentTime();
    RGB24Buffer *corrected21 = deformed->doReverseDeformationBlTyped<RadialCorrection>(&T);
    cout << "done in: " << timer.usecsToNow() << "us" << endl;

    RGB24Buffer *diff21 = RGB24Buffer::diff(image, corrected21);

    /*2.2*/
    cout << "Preparing forward deformation cache... " << flush;
    timer = PreciseTimer::currentTime();
    DisplacementBuffer *forward = new DisplacementBuffer(&T, image->h, image->w, false);
    cout << "done in: " << timer.usecsToNow() << "us" << endl;

    cout << "Applying forward deformation cache... " << flush;
    timer = PreciseTimer::currentTime();
    RGB24Buffer *corrected22 = deformed->doReverseDeformationBlTyped<DisplacementBuffer>(forward);
    cout << "done in: " << timer.usecsToNow()  << "us"  << endl;

    RGB24Buffer *diff22 = RGB24Buffer::diff(image, corrected22);


    BMPLoader().save("input.bmp"                , image);
    BMPLoader().save("forward.bmp"              , deformed);
    BMPLoader().save("backward-direct.bmp"      , corrected21);
    BMPLoader().save("backward-direct-diff.bmp" , diff21);

    BMPLoader().save("backward-cached.bmp"      , corrected22);
    BMPLoader().save("backward-cached-diff.bmp" , diff22);

    delete_safe(image);
    delete_safe(deformed);
    delete_safe(forward);
    delete_safe(inverse);
    delete_safe(diff21);
    delete_safe(diff22);
    delete_safe(corrected21);
    delete_safe(corrected22);

}


void testRadialInversion(int scale)
{
    RGB24Buffer *image = new RGB24Buffer(250 * scale, 400 * scale);

    auto operation = [](int i, int j, RGBColor *pixel)
    {
        i = i / 100;
        j = j / 200;
        if ( (i % 2) &&  (j % 2))   *pixel = RGBColor::Green();
        if (!(i % 2) &&  (j % 2))   *pixel = RGBColor::Yellow();
        if ( (i % 2) && !(j % 2))   *pixel = RGBColor::Red();
        if (!(i % 2) && !(j % 2))   *pixel = RGBColor::Blue();
    };
    touchOperationElementwize(image, operation);

    LensDistortionModelParameters deformator;
    deformator.setPrincipalX(image->w / 2);
    deformator.setPrincipalY(image->h / 2);

    deformator.setTangentialX(0.000001);
    deformator.setTangentialY(0.000001);

    deformator.setAspect(1.0);
    deformator.setScale(1.0);

    deformator.mKoeff.push_back( 0.0001);
    deformator.mKoeff.push_back(-0.00000002);
    deformator.mKoeff.push_back( 0.00000000000003);

    RadialCorrection T(deformator);
    PreciseTimer timer;

    cout << "Starting deformation... " << flush;
    timer = PreciseTimer::currentTime();
    RGB24Buffer *deformed = image->doReverseDeformationBlTyped<RadialCorrection>(&T);
    cout << "done in: " << timer.usecsToNow() << "us" << endl;

    /* */
    cout << "Starting invertion... " << flush;
    RadialCorrection invert = T.invertCorrection(image->h, image->w, 30);
    cout << "done" << endl;

    cout << "Starting backprojection... " << flush;
    timer = PreciseTimer::currentTime();
    RGB24Buffer *backproject = deformed->doReverseDeformationBlTyped<RadialCorrection>(&invert);
    cout << "done in: " << timer.usecsToNow() << "us" << endl;
    cout << "done" << endl;

    BMPLoader().save("input.bmp"      , image);
    BMPLoader().save("forward.bmp"    , deformed);
    BMPLoader().save("backproject.bmp", backproject);

    delete_safe(image);
    delete_safe(deformed);
    delete_safe(backproject);
}
