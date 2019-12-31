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

// Conversion Back to corecvs type
  FlowBuffer f(prev->h, prev->w);

  for (int i = 0; i < outBuffer.size[0]; i++){
      for (int j = 0; j < outBuffer.size[1]; j++){
          short y = outBuffer.at<Vec2f>(i, j)[0];
          short x = outBuffer.at<Vec2f>(i, j)[1];
          f.setElement(i, j, Vector2d16(y, x));
      }
  }
  return new FlowBuffer(prev->h, prev->w, f.data);
}

int PCAFlowProcessor::setFrameRGB24(RGB24Buffer *frame, int /*frameType*/) {
  prev = curr;
  curr = G8Buffer::FromG12Buffer(frame->toG12Buffer());
  return 0;
}

PCAFlowProcessor::PCAFlowProcessor(){}

int PCAFlowProcessor::setFrameG12(G12Buffer *frame, int frameType) {
    prev = curr;
    curr = G8Buffer::FromG12Buffer(frame);
    return 0;
}
