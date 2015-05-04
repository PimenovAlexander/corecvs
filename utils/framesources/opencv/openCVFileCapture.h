#pragma once
/**
 * \brief Capture video stream from avi file using OpenCV library
 */
#include <cstddef>  // GCC 4.6 needs this to compile
#include <opencv/highgui.h>

#include "imageCaptureInterface.h"

class OpenCvFileCapture : public ImageCaptureInterface
{
public:
    OpenCvFileCapture(QString const &params);
    ~OpenCvFileCapture();

    virtual ImageCaptureInterface::CapErrorCode initCapture();
    virtual ImageCaptureInterface::CapErrorCode startCapture();
    virtual FramePair getFrame();

    std::string mName;
    cv::VideoCapture mCapture;
    int count;
};
