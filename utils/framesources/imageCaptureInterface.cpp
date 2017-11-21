/**
 * \file ImageCaptureInterface.cpp
 * \brief Add Comment Here
 *
 * \date Mar 14, 2010
 * \author alexander
 */
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "core/utils/global.h"

#include "imageCaptureInterface.h"
#include "cameraControlParameters.h"

#ifdef WITH_FRAMESOURCE_V4L2
# include "V4L2Capture.h"
# include "V4L2CaptureDecouple.h"
#endif

char const *CaptureStatistics::names[] =
{
    "Desync time",
    "Internal desync",
    "Decoding time",
    "Interframe delay",
    "Frame Data size"
};

STATIC_ASSERT(CORE_COUNT_OF(CaptureStatistics::names) == CaptureStatistics::MAX_ID, wrong_comment_num_capture_stats)




void ImageCaptureInterface::notifyAboutNewFrame(frame_data_t frameData)
{
    SYNC_PRINT(("ImageCaptureInterface::notifyAboutNewFrame()\n"));
    if (imageInterfaceReceiver != NULL)
    {
        imageInterfaceReceiver->newFrameReadyCallback(frameData);
        imageInterfaceReceiver->newImageReadyCallback();
    } else {
        SYNC_PRINT(("Warning: ImageCaptureInterface::notifyAboutNewFrame(): imageInterfaceReceiver is NULL\n"));
    }
}

ImageCaptureInterface::ImageCaptureInterface()
   : mIsRgb(false)
{

    imageInterfaceReceiver = NULL;
    SYNC_PRINT(("ImageCaptureInterface::ImageCaptureInterface(): called\n"));
}

ImageCaptureInterface::~ImageCaptureInterface()
{
    return;
}

void ImageCaptureInterface::getAllCameras(vector<string> &cameras)
{
#ifdef Q_OS_WIN
# ifdef WITH_DIRECTSHOW
    vector<string> dshowcams;
    DirectShowCaptureInterface::getAllCameras(dshowcams);
    for (const string& cam : dshowcams) {
        cameras.push_back(std::string("dshow:" + cam));
    }
# endif
#endif

#ifdef WITH_FRAMESOURCE_V4L2
    vector<string> v4lcams;
    V4L2CaptureInterface::getAllCameras(v4lcams);
    for (string cam: v4lcams) {
        cameras.push_back(std::string("v4l2:" + cam));
    }
#endif
#ifdef WITH_UEYE
    vector<string> ueyecams;
    UEyeCaptureInterface::getAllCameras(ueyecams);
    for (string cam: ueyecams) {
        cameras.push_back(std::string("ueye:" + cam));
    }
#endif
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::setCaptureProperty(int /*id*/, int /*value*/)
{
    return FAILURE;
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::getCaptureProperty(int /*id*/, int * /*value*/)
{
    return FAILURE;
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::getCaptureName(std::string & /*value*/)
{
    return FAILURE;
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::getFormats(int * /*num*/, CameraFormat *& /*formats*/)
{
    return FAILURE;
}

bool ImageCaptureInterface::getCurrentFormat(ImageCaptureInterface::CameraFormat & /*format*/)
{
    return false;
}

std::string ImageCaptureInterface::getInterfaceName()
{
    return "";
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::getDeviceName(int /*num*/, std::string & /*name*/)
{
    return FAILURE;
}

std::string ImageCaptureInterface::getDeviceSerial(int /*num*/)
{
    return "";
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::initCapture()
{
    return FAILURE;
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::startCapture()
{
    return FAILURE;
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::pauseCapture()
{
    return FAILURE;
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::nextFrame()
{
    return FAILURE;
}

ImageCaptureInterface::CapErrorCode ImageCaptureInterface::queryCameraParameters(CameraParameters &parameter)
{
    CORE_UNUSED(parameter);
    return ImageCaptureInterface::SUCCESS;
}

bool ImageCaptureInterface::supportPause()
{
    return false;
}
