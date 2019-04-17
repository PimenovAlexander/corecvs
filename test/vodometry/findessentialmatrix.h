#pragma once

#include "core/math/matrix/matrix33.h"
#include "core/buffers/flow/flowBuffer.h"

#include <opencv/cv.h>
#include "core/buffers/flow/flowVector.h"

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include "convertors.h"

void findEssentialMatrix(corecvs::Matrix33 *E, corecvs::FlowBuffer *flow) {
    vector<cv::Point2f> points1;
    vector<cv::Point2f> points2;

    convertToOpenCVFlowBuffer(flow, &points1, &points2);
    cv::Mat Es = cv::findEssentialMat(points1, points2);
    *E = getCoreCVSMatrix33(Es);
}

void findRt(corecvs::Matrix33 *R, corecvs::Vector3dd *t, corecvs::Matrix33 E, corecvs::FlowBuffer *flow) {
    cv::Mat Es = getOpenCVMat(E);
    vector<cv::Point2f> points1;
    vector<cv::Point2f> points2;
    convertToOpenCVFlowBuffer(flow, &points1, &points2);
    cv::Mat Ro, tr;

    cv::recoverPose(Es, points1, points2, Ro, tr);

    *R = getCoreCVSMatrix33(Ro);
    *t = getCoreCVSVector(tr);
}
