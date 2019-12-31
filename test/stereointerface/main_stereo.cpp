//
// Created by elijah on 24.12.2019.
//
#include <core/stereointerface/PCAFlowProcessor.h>
#include "core/stereointerface/processor6D.h"
#include <opencv2/optflow/pcaflow.hpp>
#include <fstream>
#include <core/fileformats/bmpLoader.h>
#include <opencv2/imgcodecs.hpp>

using namespace std;
using namespace corecvs;
using namespace cv;

int main(int argc, char** argv) {
    auto loader = BMPLoaderRGB24();
    auto image_1 = *loader.loadRGB("/corecvs/test_1.bmp");
    auto image_2 = *loader.loadRGB("/corecvs/test_2.bmp");
    auto processor = PCAFlowProcessor();
    processor.setFrameRGB24(&image_1, 1);
    processor.endFrame();
    processor.setFrameRGB24(&image_2, 1);
    processor.endFrame();
    auto flow_buffer = processor.opticalFlow;

    image_1.drawFlowBuffer(flow_buffer);

    cout << "Save result: " << loader.save("/corecvs/out.bmp", &image_1) << endl;

    return 0;
}