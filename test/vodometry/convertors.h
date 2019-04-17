#pragma once

#include <vector>
#include <opencv/cv.h>
#include "core/buffers/flow/floatFlowBuffer.h"

void convertToOpenCVFlowBuffer(corecvs::FlowBuffer *flow, vector<cv::Point2f> *points1, vector<cv::Point2f> *points2);
cv::Mat getOpenCVMat(corecvs::Matrix33);
cv::Point2f getOpenCVPoint(corecvs::Vector2dd);

corecvs::Matrix33 getCoreCVSMatrix33(cv::Mat);
corecvs::Vector3dd getCoreCVSVector(cv::Mat);
