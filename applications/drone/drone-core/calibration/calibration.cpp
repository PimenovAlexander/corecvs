#include "calibration.h"
#include <opencv2/imgproc.hpp>


Calibration::Calibration()
{

}

Calibration::Calibration(cv::Mat intrinsicMat, cv::Mat distCoeffsMat)
{
    setIntrinsicMat(intrinsicMat);
    setDistCoeffsMat(distCoeffsMat);
}

void Calibration::setIntrinsicMat(cv::Mat intristicMat)
{
    intrinsic = intristicMat;
    intrinsicSet = true;
}

void Calibration::setDistCoeffsMat(cv::Mat distCoeffsMat)
{
    distCoeffs = distCoeffsMat;
    distCoeffsSet = true;
}

cv::Mat Calibration::calibrateImage(cv::Mat image)
{
    cv::Mat imageUndistorted;
    if (intrinsicSet && distCoeffsSet)
    {
    undistort(image, imageUndistorted, intrinsic, distCoeffs);
    }
    else {
        image.copyTo(imageUndistorted);

    }
    return imageUndistorted;
}

cv::Mat Calibration::calibrateImage(cv::Mat image, cv::Mat intrinsicMat,cv::Mat distCoeffsMat)
{
    cv::Mat imageUndistorted;
    undistort(image, imageUndistorted, intrinsicMat, distCoeffsMat);
    return imageUndistorted;
}
