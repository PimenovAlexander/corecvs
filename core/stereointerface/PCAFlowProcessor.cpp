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
  if (curr != NULL && prev != NULL) {
    delete_safe(opticalFlow);
    opticalFlow = getFlow();
  }
  prev = curr;
  return 0;
}

FlowBuffer *PCAFlowProcessor::getFlow() {
  auto *Processor = new optflow::OpticalFlowPCAFlow();
  auto outBuffer = Mat(prev->h, prev->w, CV_8U); // output for openCV algorithm
  // Conversion images to openCV type
  auto inBuffer_prev = Mat(prev->h, prev->w, CV_8U, prev->data);
  auto inBuffer_curr = Mat(curr->h, curr->w, CV_8U, curr->data);

  Processor->calc(
      inBuffer_prev,
      inBuffer_curr,
      outBuffer);
  return new FlowBuffer(prev->h, prev->w, outBuffer.data); // Conversion back to corecvs buffer type
}

int PCAFlowProcessor::setFrameRGB24(RGB24Buffer *frame, int /*frameType*/) {
  prev = curr;
  curr = G8Buffer::FromG12Buffer(frame->toG12Buffer());
  return 0;
}

PCAFlowProcessor::PCAFlowProcessor(){}