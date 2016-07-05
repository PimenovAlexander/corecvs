#pragma once
/**
 * \brief Capture video stream from avi file using OpenCV library
 */
#include <cstddef>  // GCC 4.6 needs this to compile

#include <opencv2/highgui/highgui.hpp>  // cv::VideoCapture

#include "imageCaptureInterface.h"

class OpenCvFileCapture : public ImageCaptureInterface
{
public:
    OpenCvFileCapture(QString const &params);
   ~OpenCvFileCapture();

    virtual ImageCaptureInterface::CapErrorCode initCapture() override;
    virtual ImageCaptureInterface::CapErrorCode startCapture() override;
    virtual FramePair getFrame() override;

    std::string mName;
    cv::VideoCapture mCapture;
    int count;
};
