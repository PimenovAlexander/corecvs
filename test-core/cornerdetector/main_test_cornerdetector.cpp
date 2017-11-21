/**
 * \file main_test_cornerdetector.cpp
 * \brief This is the main file for the test cornerdetector
 *
 * \date Oct 21, 2010
 * \author alexander
 *
 * \ingroup autotest
 */

#include <iostream>
#include <memory>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "core/buffers/g12Buffer.h"
#include "core/buffers/kernels/spatialGradient.h"
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/bmpLoader.h"
#include "core/segmentation/segmentator.h"
#include "core/buffers/rgb24/rgb24Buffer.h"


#include "core/patterndetection/chessBoardCornerDetector.h"


using namespace corecvs;
using namespace std;

TEST(Cornerdetector, DISABLED_testCornerDetector)
{
    G12Buffer *input = BufferFactory::getInstance()->loadG12Bitmap("data/calib-object.bmp");
    if (input == nullptr)
    {
        cout << "Could not open test image" << endl;
        return;
    }
    SpatialGradient *grad = new SpatialGradient(input);
    G12Buffer *corners = grad->findCornerPoints(180.0);


    CornerSegmentator::SegmentationResult *result;
    CornerSegmentator segmentator(G12Buffer::BUFFER_MAX_VALUE * 4 / 5);
    result = segmentator.segment<G12Buffer>(corners);

    RGB24Buffer *show = new RGB24Buffer(corners);
    for (unsigned i = 0; i < result->segments->size(); i++)
    {
        Vector2dd center = result->segments->at(i)->getMean();
        cout << center << endl;
        show->drawCrosshare2(fround(center.x()), fround(center.y()), RGBColor(0xFF0000));
    }


    (BMPLoader()).save("corners.bmp", show);
    delete show;
    delete input;
    delete grad;
    delete corners;
    delete result;
}


TEST(Cornerdetector, DISABLED_testChessCornerDetector)
{
    unique_ptr<RGB24Buffer> input(BufferFactory::getInstance()->loadRGB24Bitmap("data/calib-object.bmp"));
    if (input.get() == nullptr)
    {
        cout << "Could not open test image" << endl;
        return;
    }

    DpImage grayscale(input->getSize());
    grayscale.binaryOperationInPlace(*input, [](const double & /*a*/, const corecvs::RGBColor &b) {
        return b.yd() / 255.0;
    });

    vector<OrientedCorner> corners;

    ChessBoardCornerDetectorParams params;

    params.setProduceDebug(true);
    ChessBoardCornerDetector detector(params);

    detector.detectCorners(grayscale, corners);

    for (std::string name: detector.debugBuffers())
    {
        unique_ptr<RGB24Buffer> debug(detector.getDebugBuffer(name));
        BMPLoader().save(name, debug.get());
    }
}

