#pragma once
/**
 * \brief Capture video stream from avi file using OpenCV library
 */
#include <cstddef>  // GCC 4.6 needs this to compile

#include <opencv2/highgui/highgui.hpp>  // cv::VideoCapture

#include "core/framesources/imageCaptureInterface.h"

class OpenCvFileCapture : public virtual ImageCaptureInterface
{
public:
    OpenCvFileCapture(const std::string &params);
   ~OpenCvFileCapture();

    virtual ImageCaptureInterface::CapErrorCode initCapture() override;
    virtual ImageCaptureInterface::CapErrorCode startCapture() override;
    virtual FramePair getFrame() override;

    std::string mName;
    cv::VideoCapture mCapture;
    int count;
};

class OpenCvFileCaptureProducer : public ImageCaptureInterfaceProducer
{
public:
    OpenCvFileCaptureProducer()
    {}

    virtual std::string getPrefix() override
    {
        return "opencv_file:";
    }

    virtual ImageCaptureInterface *produce(std::string &name, bool /*isRGB*/) override
    {
        return new OpenCvFileCapture(name);
    }
};
