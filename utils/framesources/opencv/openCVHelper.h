#pragma once

/* GCC 4.6 needs this to compile*/
#include <cstddef>

#include <opencv2/highgui/highgui.hpp>

#include "g12Buffer.h"

using corecvs::G12Buffer;

class OpenCvHelper
{
public:
    static bool captureImageCopyToBuffer(cv::VideoCapture &capture, G12Buffer* buf);
    static bool captureImageCopyToBuffer(CvCapture* capHandle, G12Buffer* buf);
private:
    static bool captureImageCopyToBuffer(IplImage* image, G12Buffer* buf);
};
