/**
 * \file main_test_histogram.cpp
 * \brief This is the main file for the test histogram 
 *
 * \date Feb 23, 2012
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>

#include "g12Buffer.h"
#include "histogram.h"
#include "bmpLoader.h"
#include "bufferFactory.h"

#include "global.h"

using namespace std;
using namespace corecvs;

void testBinarization (void)
{
    G12Buffer *image = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");

    Histogram *histogram = new Histogram(image);

    int meanLevel   = histogram->getMeanThreshold();
    int medianLevel = histogram->getMedianThreshold();
    int otsuLevel   = histogram->getOtsuThreshold();

    G12Buffer *meanImage   = image->binarize(meanLevel);
    BMPLoader().save("mean.bmp", meanImage);

    G12Buffer *medianImage = image->binarize(medianLevel);
    BMPLoader().save("median.bmp", medianImage);

    G12Buffer *otsuImage   = image->binarize(otsuLevel);
    BMPLoader().save("otsu.bmp", otsuImage);

    delete otsuImage;
    delete medianImage;
    delete meanImage;
    delete histogram;
    delete image;

}

int main (int /*argC*/, char ** /*argV*/)
{
    testBinarization ();
    cout << "PASSED" << endl;
    return 0;
}
