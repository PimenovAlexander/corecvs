#include "ertfinder.h"

ertFinder::ertFinder(double focal, double x, double y) {
    this->focal = focal;
    this->x = x;
    this->y = y;
}

void ertFinder::findEssentialMatrix(corecvs::Matrix33 *E, corecvs::FlowBuffer *flow) {
    vector<cv::Point2f> points1;
    vector<cv::Point2f> points2;

    convertToOpenCVFlowBuffer(flow, &points1, &points2);

    cv::Point2d pp(this->x, this->y);
    cv::Mat Es = cv::findEssentialMat(points1, points2, this->focal, pp, cv::RANSAC, 0.999, 1.0);
    *E = getCoreCVSMatrix33(Es);
}

void ertFinder::findRt(corecvs::Matrix33 *R, corecvs::Vector3dd *t, corecvs::Matrix33 E, corecvs::FlowBuffer *flow) {
    cv::Mat Es = getOpenCVMat(E);
    vector<cv::Point2f> points1;
    vector<cv::Point2f> points2;
    convertToOpenCVFlowBuffer(flow, &points1, &points2);
    cv::Mat Ro, tr;

    cv::Point2d pp(this->x, this->y);
    cv::recoverPose(Es, points1, points2, Ro, tr, this->focal, pp);

    *R = getCoreCVSMatrix33(Ro);
    *t = getCoreCVSVector(tr);
}
