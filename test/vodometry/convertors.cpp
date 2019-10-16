#include "convertors.h"


/* neednt all of them */
#include <opencv/cv.h>
#include "core/buffers/flow/flowVector.h"

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

/* */

#include "core/math/matrix/matrix33.h"

void convertToOpenCVFlowBuffer(corecvs::FlowBuffer *flow, vector<cv::Point2f> *points1, vector<cv::Point2f> *points2) {
    vector<corecvs::FloatFlowVector> *flowVectors = flow->toVectorForm();
    int size = flowVectors->size();

    for (int i = 0; i < size; i++) {
        corecvs::FloatFlowVector cur = flowVectors->at(i);
        points1->push_back(getOpenCVPoint(cur.start));
        points2->push_back(getOpenCVPoint(cur.end));
    }
}

cv::Mat getOpenCVMat(corecvs::Matrix33 from) {
    cv::Mat to = cv::Mat(3,3, CV_64F);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            to.at<double>(i, j) = from.a(i, j);
        }
    }

    return to;
}

cv::Point2f getOpenCVPoint(corecvs::Vector2dd v) {
    return cv::Point2f(v.x(), v.y());
}

/* ignore cases when input matrix is not 3x3 */
corecvs::Matrix33 getCoreCVSMatrix33(cv::Mat from) {
    corecvs::Matrix33 m = corecvs::Matrix33::Identity();

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            m.a(i, j) = from.at<double>(i, j);
        }
    }
    return m;
}

/* ignore cases when input matrix is not 1x3 */
corecvs::Vector3dd getCoreCVSVector(cv::Mat from) {
    return corecvs::Vector3dd(from.at<double>(0, 0), from.at<double>(0, 1), from.at<double>(0, 2));
}
