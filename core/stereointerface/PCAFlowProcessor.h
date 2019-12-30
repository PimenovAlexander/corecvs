//
// Created by elijah on 24.12.2019.
//

#ifndef CORECVS_PCAFLOWPROCESSOR_H
#define CORECVS_PCAFLOWPROCESSOR_H

#endif //CORECVS_PCAFLOWPROCESSOR_H

#include <opencv2/optflow/pcaflow.hpp>
#include "core/stereointerface/processor6D.h"
using namespace corecvs;
using namespace cv;

class PCAFlowProcessor{
    G8Buffer *prev  = NULL;
    G8Buffer *curr  = NULL;

public:
    FlowBuffer *opticalFlow = NULL;
    PCAFlowProcessor();
    int beginFrame();
    int endFrame();
    FlowBuffer *getFlow();
    int setFrameRGB24(RGB24Buffer *frame, int /*frameType*/);
    int setFrameG12(G12Buffer *frame, int frameType);
};