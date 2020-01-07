//
// Created by elijah on 24.12.2019.
//

#include "PCAFlowProcessor.h"
#include "core/stereointerface/processor6D.h"
#include <opencv2/optflow/pcaflow.hpp>
#include <core/fileformats/bmpLoader.h>
using namespace corecvs;
using namespace cv;

int PCAFlowProcessor::beginFrame() { return 0; }

int PCAFlowProcessor::endFrame() {
    SYNC_PRINT(("endFrame()\n"));
  if (curr != NULL && prev != NULL) {
    delete_safe(opticalFlow);
    calculateFlow();
  }
  return 0;
}

void PCAFlowProcessor::calculateFlow() {
    // TODO: err handling
    // TODO: param handling
    // TODO: logging
    SYNC_PRINT(("calculateFlow()\n"));
    auto *Processor = new optflow::OpticalFlowPCAFlow();
    auto outBuffer = Mat(prev->h, prev->w, CV_8U); // output for openCV algorithm
    // Conversion images to openCV type
    auto inBuffer_prev = Mat(prev->h, prev->w, CV_8U, prev->data);
    auto inBuffer_curr = Mat(curr->h, curr->w, CV_8U, curr->data);

    Processor->calc(
            inBuffer_prev,
            inBuffer_curr,
            outBuffer);

// Conversion Back to corecvs type
    opticalFlow = new FlowBuffer(prev->h, prev->w);

    for (int i = 0; i < outBuffer.size[0]; i++){
        for (int j = 0; j < outBuffer.size[1]; j++){
            short y = outBuffer.at<Vec2f>(i, j)[0];
            short x = outBuffer.at<Vec2f>(i, j)[1];
            opticalFlow->setElement(i, j, Vector2d16(y, x));
        }
    }

    return;
}

FlowBuffer *PCAFlowProcessor::getFlow() {
    return opticalFlow;
}

int PCAFlowProcessor::setFrameRGB24(RGB24Buffer *frame, int /*frameType*/) {
  delete_safe(prev);
  prev = curr;
  auto tmp = frame->toG12Buffer();
  curr = G8Buffer::FromG12Buffer(tmp);
  delete tmp;

  return 0;
}

PCAFlowProcessor::PCAFlowProcessor(){}

int PCAFlowProcessor::setFrameG12(G12Buffer *frame, int frameType) {
    delete_safe(prev);
    prev = curr;
    curr = G8Buffer::FromG12Buffer(frame);
    return 0;
}

PCAFlowProcessor::~PCAFlowProcessor() {
    delete_safe(curr);
    delete_safe(prev);
    delete_safe(opticalFlow);
}

