#ifndef OPENCVKEYPOINTSWRAPPER_H
#define OPENCVKEYPOINTSWRAPPER_H

#include "core/features2d/imageKeyPoints.h"
#include "core/features2d/imageMatches.h"
#include <opencv2/features2d/features2d.hpp>

cv::KeyPoint convert(const KeyPoint &kp);

KeyPoint convert(const cv::KeyPoint &kp);

cv::DMatch convert(const RawMatch &rm);

RawMatch convert(const cv::DMatch &dm);

cv::Mat convert(const corecvs::RuntimeTypeBuffer &buffer);

corecvs::RuntimeTypeBuffer convert(const cv::Mat &mat);

#endif
