/**
 * \brief Capture video stream from avi file using OpenCV library
 */
#include <QtCore/QRegExp>
#include <QtCore/QString>

#include <opencv/highgui.h>
#include "openCvFileCapture.h"
#include "openCvHelper.h"

OpenCvFileCapture::OpenCvFileCapture(QString const &params)
    : AbstractFileCapture(params)
{
    mCapture.open(mPathFmt);

    if (!mCapture.isOpened())
    {
        printf("failed to open file");
        exit(EXIT_FAILURE);  // TODO: do not use exit() here.
    }

    mSpin = new SpinThread(this, mDelay, mCurrent, mCapture, mPathFmt);
}

ImageCaptureInterface::FramePair OpenCvFileCapture::getFrame()
{
    mProtectFrame.lock();
        FramePair result(NULL, NULL);
        result.bufferLeft     = new G12Buffer(mCurrent.bufferLeft);
        result.bufferRight    = new G12Buffer(mCurrent.bufferRight);
        result.leftTimeStamp  = mCurrent.leftTimeStamp;
        result.rightTimeStamp = mCurrent.rightTimeStamp;
    mProtectFrame.unlock();
    return result;
}

ImageCaptureInterface::CapErrorCode OpenCvFileCapture::startCapture()
{
    mSpin->start();
//  return ImageCaptureInterface::CapSuccess1Cam;
    return ImageCaptureInterface::SUCCESS;
}

OpenCvFileCapture::SpinThread::SpinThread(AbstractFileCapture *pInterface
    , int delay
    , ImageCaptureInterface::FramePair &framePair
    , cv::VideoCapture &capture
    , string const &path
    )
    : AbstractFileCaptureSpinThread(pInterface, delay, framePair)
    , mCapture(capture)
    , mPath(path)
    , mTry(0)
{
}

bool OpenCvFileCapture::SpinThread::grabFramePair()
{
    uint width  = mCapture.get(CV_CAP_PROP_FRAME_WIDTH);
    uint height = mCapture.get(CV_CAP_PROP_FRAME_HEIGHT);

    delete mFramePair.bufferLeft;
    delete mFramePair.bufferRight;

    mFramePair.bufferLeft = new G12Buffer(height, width, false);
    mFramePair.bufferRight = NULL;

    // OpenCV does not set timestamps for the frames
    mFramePair.leftTimeStamp  = 0;
    mFramePair.rightTimeStamp = 0;

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
    if (mTry < maxNumberOfTries)
        return grabFramePair();

    return false;
}
