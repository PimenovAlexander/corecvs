#ifndef CALIBRATION_H
#define CALIBRATION_H
#include "opencv2/core.hpp"

class Calibration
{
public:
    Calibration();
    cv::Mat calibrateImage(cv::Mat image);
    static cv::Mat calibrateImage(cv::Mat image, cv::Mat intrinsicMat, cv::Mat distCoeffsMat);
    void setIntrinsicMat(cv::Mat intristicMat);
    void setDistCoeffsMat(cv::Mat distCoeffsMat);

    Calibration(cv::Mat intristicMat, cv::Mat distCoeffsMat);
private:
    bool intrinsicSet = false;
    bool distCoeffsSet = false;
    cv::Mat intrinsic;
    cv::Mat distCoeffs;
};

#endif // CALIBRATION_H
