//
// Created by elijah on 24.12.2019.
//

#include "PCAFlowProcessor.h"
#include "generated/openCVPCAFlowParameters.h"
#include "core/stereointerface/processor6D.h"
#include <opencv2/optflow/pcaflow.hpp>
#include <core/fileformats/bmpLoader.h>
using namespace corecvs;
using namespace cv;

int PCAFlowProcessor::beginFrame() { return 0; }

int PCAFlowProcessor::endFrame() {
  if (curr != NULL && prev != NULL) {
    delete_safe(opticalFlow);

    if (calculateFlow() != 0U) {
        return -1;
    };
  }
  return 0;
}

int PCAFlowProcessor::calculateFlow() {

    if (prev->getSize() != curr->getSize()) {
        return -1;
    }

    cout << "PCAFlowProcessor::calculateFlow(): Calculating flow of size [" << curr->w << "x"  << curr->h << "]...\n";

    auto outBuffer = Mat(prev->h, prev->w, CV_8U); // output for openCV algorithm
    /* Conversion to openCV data structures */
    auto inBuffer_prev = Mat(prev->h, prev->w, CV_8U, prev->data);
    auto inBuffer_curr = Mat(curr->h, curr->w, CV_8U, curr->data);


    try {
        /* Actual processing */
        auto *Processor = new optflow::OpticalFlowPCAFlow(
                Ptr<optflow::PCAPrior>(),
                cv::Size(params.basisWidth(), params.basisHeight()),
                params.sparseRate(),
                params.retainedCornersFraction(),
                params.occlusionsThreshold(),
                params.dampingFactor(),
                params.claheClip()
        );

        Processor->calc(
                inBuffer_prev,
                inBuffer_curr,
                outBuffer);

    } catch (cv::Exception e) {
        cout << "PCAFlowProcessor::calculateFlow(): exception in wrapped class:" << endl;
        cout << e.msg << endl;
        cout.flush();
        return -1;
    }


    /* Conversion back to corecvs structures */
    opticalFlow = new FlowBuffer(prev->h, prev->w);

    for (int i = 0; i < outBuffer.size[0]; i++){
        for (int j = 0; j < outBuffer.size[1]; j++){
            short y = outBuffer.at<Vec2f>(i, j)[0];
            short x = outBuffer.at<Vec2f>(i, j)[1];
            opticalFlow->setElement(i, j, Vector2d16(y, x));
        }
    }

    cout << "PCAFlowProcessor::calculateFlow(): flow calculated" << endl;

    return 0;
}

FlowBuffer *PCAFlowProcessor::getFlow() {
    return opticalFlow;
}

int PCAFlowProcessor::setFrameRGB24(RGB24Buffer *frame, int frameType) {
  auto tmp = frame->toG12Buffer();
  auto r = setFrameG12(tmp, frameType);
  delete tmp;
  return r;
}

PCAFlowProcessor::PCAFlowProcessor(){}

int PCAFlowProcessor::setFrameG12(G12Buffer *frame, int frameType) {
    delete_safe(prev);
    prev = curr;
    curr = G8Buffer::FromG12Buffer(frame);
    cout << "PCAFlowProcessor::setFrameG12: set frame of size [" << curr->w << "x" << curr->h << "]" << endl;
    return 0;
}

PCAFlowProcessor::~PCAFlowProcessor() {
    delete_safe(curr);
    delete_safe(prev);
    delete_safe(opticalFlow);
}

std::map<std::string, DynamicObject> PCAFlowProcessor::getParameters() {
    auto r = std::map<std::string, DynamicObject>();
    r.emplace("pca", DynamicObject(&params));
    return r;
}

bool PCAFlowProcessor::setParameters(std::string name, const DynamicObject &param) {
    param.copyTo<OpenCVPCAFlowParameters>(&params);
    return true;
}
