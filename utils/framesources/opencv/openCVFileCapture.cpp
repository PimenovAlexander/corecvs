/**
 * \brief Capture video stream from avi file using OpenCV library
 */

#include "openCVFileCapture.h"
//#include "openCVHelper.h"
#include "../../../wrappers/opencv/openCVTools.h"

OpenCvFileCapture::OpenCvFileCapture(const  std::string &params)
    : /*AbstractFileCapture(params),*/
       mName(params)
     , count(1)
{
    SYNC_PRINT(("OpenCvFileCapture::OpenCvFileCapture(%s): called\n", params.c_str()));

    SYNC_PRINT(("OpenCvFileCapture::OpenCvFileCapture(): exited\n"));
}

ImageCaptureInterface::CapErrorCode OpenCvFileCapture::initCapture()
{
    SYNC_PRINT(("OpenCvFileCapture::initCapture(): called\n"));

    //mCapture.open(mPathFmt);
    mCapture.open(mName);

    if (!mCapture.isOpened())
    {
        SYNC_PRINT(("OpenCvFileCapture::initCapture(): failed to open file"));
        printf("failed to open file");

        return ImageCaptureInterface::FAILURE;
    }

    SYNC_PRINT(("OpenCvFileCapture::initCapture(): exited\n"));
    return ImageCaptureInterface::SUCCESS_1CAM;
}

ImageCaptureInterface::FramePair OpenCvFileCapture::getFrame()
{
    SYNC_PRINT(("OpenCvFileCapture::getFrame(): called\n"));
    //mProtectFrame.lock();
        FramePair result;

        mCapture.grab();
        cv::Mat image;
        mCapture.retrieve(image);

        result.setRgbBufferLeft  (OpenCVTools::getRGB24BufferFromCVMat(image));
        result.setRgbBufferRight (new RGB24Buffer(result.rgbBufferLeft()));

        result.setBufferLeft     (result.rgbBufferLeft()->toG12Buffer());
        result.setBufferRight    (new G12Buffer(result.bufferLeft()));

        /*for (int i = 0; i < result.bufferLeft->h; i++)
        {
            for (int j = 0; j < result.bufferLeft->w; j++)
            {
                result.bufferLeft->element(i,j) = ((i % 2) ^ (j % 2) ^ (count % 2)) ? 0 : G12Buffer::BUFFER_MAX_VALUE;
            }
        }*/

        result.setTimeStampLeft (count * 10);
        result.setTimeStampRight(count * 10);

    //mProtectFrame.unlock();

    count++;
    ImageCaptureInterface::FrameMetadata frameData;
    frameData.timestamp = (count * 10);
    notifyAboutNewFrame(frameData);
    //count++;

    return result;
}

ImageCaptureInterface::CapErrorCode OpenCvFileCapture::startCapture()
{
//  return ImageCaptureInterface::CapSuccess1Cam;
    SYNC_PRINT(("OpenCvFileCapture::startCapture(): called\n"));
    ImageCaptureInterface::FrameMetadata frameData;
    frameData.timestamp = (count * 10);

    SYNC_PRINT(("OpenCvFileCapture::startCapture(): sending notification\n"));
    notifyAboutNewFrame(frameData);
    count++;

    SYNC_PRINT(("OpenCvFileCapture::startCapture(): exited\n"));
    return ImageCaptureInterface::SUCCESS;
}

OpenCvFileCapture::~OpenCvFileCapture()
{
    mCapture.release();
}

#if 0
bool OpenCvFileCapture::SpinThread::grabFramePair()
{
    SYNC_PRINT(("OpenCvFileCapture::SpinThread::grabFramePair(): called\n"));

    uint width  = mCapture.get(CV_CAP_PROP_FRAME_WIDTH);
    uint height = mCapture.get(CV_CAP_PROP_FRAME_HEIGHT);

    delete_safe (mFramePair.bufferLeft);
    delete_safe (mFramePair.bufferRight);

    mFramePair.bufferLeft = new G12Buffer(height, width, false);
    mFramePair.bufferRight = NULL;

    // OpenCV does not set timestamps for the frames
    static int count = 0;

    mFramePair.leftTimeStamp  = count * 10;
    mFramePair.rightTimeStamp = count * 10;
    count++;

    if (OpenCvHelper::captureImageCopyToBuffer(mCapture, mFramePair.bufferLeft))
    {
        mFramePair.bufferRight = new G12Buffer(mFramePair.bufferLeft);
        mTry = 0;
        return true;
    }

    // Restart capture
    pause();
    mCapture.release();
    mCapture.open(mPath);
    ++mTry;
    if (mTry < maxNumberOfTries) {
        return grabFramePair();
    }

    return false;
}
#endif
