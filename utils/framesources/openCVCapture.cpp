/**
 * \file OpenCVCapture.cpp
 * \brief Capture camera video stream using OpenCV library
 *
 * \date Jul 15, 2010
 * \author sergeyle
 */
#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <opencv/highgui.h>

#include "global.h"

#include "openCVCapture.h"
#include "openCvHelper.h"

OpenCVCaptureInterface::OpenCVCaptureInterface(string _devname,  unsigned int mode) : spin(this)
{
    unsigned int domain = 0;

    if      (mode == CAP_ANY)
    {
        domain = CV_CAP_ANY;
        printf("OpenCVCapture: using ANY. Input: %s\n", _devname.c_str());
        fflush(stdout);
    }
    else if (mode == CAP_VFW)
    {
        domain = CV_CAP_VFW;
        printf("OpenCVCapture: using VFW. Input: %s\n", _devname.c_str());
        fflush(stdout);
    }
    else if (mode == CAP_DS)
    {
        domain = CV_CAP_DSHOW;
        printf("OpenCVCapture: using DirectShow. Input: %s\n", _devname.c_str());
        fflush(stdout);
    }

    QRegExp devStringPattern(QString("^(\\d*)(,(\\d*))?(,(\\d*)ms)?$"));
    static const int DEVICE_1_GROUP  = 1;
    static const int DEVICE_2_GROUP  = 3;
    static const int DELAY_GROUP     = 5;

    QString qdevname = QString::fromStdString(_devname);
    int result = devStringPattern.indexIn(qdevname);
    if (result == -1)
    {
        printf("Error in device string format <%s>\n", _devname.c_str());
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    bool err;
    int left_index  = devStringPattern.cap(DEVICE_1_GROUP).toInt(&err);
    int right_index = devStringPattern.cap(DEVICE_2_GROUP).toInt(&err);

    captureLeft  = 0;
    captureRight = 0;

    int id1 = left_index  + domain;
    int id2 = right_index + domain;

    SYNC_PRINT(("Calling first  cvCaptureFromCAM(%d)\n", id1));
    captureLeft = cvCaptureFromCAM(id1);
    SYNC_PRINT(("Calling second cvCaptureFromCAM(%d)\n", id1));
    captureRight = cvCaptureFromCAM(id2);


    if (!captureLeft && !captureRight)
    {
        printf("Both cameras failed to initialize");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    // We assume that if there is only one camera active, it is the left camera
    if (!captureLeft && captureRight)
    {
        captureLeft  = captureRight;
        captureRight = 0;
    }

    delay = devStringPattern.cap(DELAY_GROUP).toInt(&err);
    if (!err)
        delay = CAP_DEFAULT_DELAY;

    current.bufferLeft  = new G12Buffer(800, 600, false);
    current.bufferRight = new G12Buffer(800, 600, false);
}

void OpenCVCaptureInterface::SpinThread::run()
{
    // We must capture a frame before getting properties.
    cvQueryFrame(mInterface->captureLeft);

    while (!mStopping)
    {
        uint width  = cvGetCaptureProperty(mInterface->captureLeft, CV_CAP_PROP_FRAME_WIDTH);
        uint height = cvGetCaptureProperty(mInterface->captureLeft, CV_CAP_PROP_FRAME_HEIGHT);

        OpenCVCaptureInterface::FramePair *pair = &(mInterface->current);

        mInterface->protectFrame.lock();
            delete pair->bufferLeft;
            delete pair->bufferRight;
            pair->bufferLeft  = new G12Buffer(height, width, false);
            pair->bufferRight = new G12Buffer(height, width, false);

            // OpenCV does not set timestamps for the frames
            pair->leftTimeStamp  = 0;
            pair->rightTimeStamp = 0;

            if (mInterface->captureLeft)
            {
                OpenCvHelper::captureImageCopyToBuffer(mInterface->captureLeft, pair->bufferLeft);
            }

            if (mInterface->captureRight)
            {
                OpenCvHelper::captureImageCopyToBuffer(mInterface->captureRight, pair->bufferRight);
            }
        mInterface->protectFrame.unlock();

        frame_data_t frameData;
        frameData.timestamp = (pair->leftTimeStamp / 2) + (pair->rightTimeStamp / 2);
        mInterface->notifyAboutNewFrame(frameData);

        /*
         * If we don't sleep here we'll find ourselves flooding the application with false new
         * frame notifications
         */
        if (mInterface->delay)
            msleep(mInterface->delay);
    }
}

void OpenCVCaptureInterface::SpinThread::stop()
{
    mStopping = true;
}

OpenCVCaptureInterface::FramePair OpenCVCaptureInterface::getFrame()
{
    protectFrame.lock();
        FramePair result;
        result.bufferLeft     = new G12Buffer(current.bufferLeft);
        result.bufferRight    = new G12Buffer(current.bufferRight);
        result.leftTimeStamp  = current.leftTimeStamp;
        result.rightTimeStamp = current.rightTimeStamp;
    protectFrame.unlock();
    return result;
}

ImageCaptureInterface::CapErrorCode OpenCVCaptureInterface::setCaptureProperty(int id, int value )
{
    CORE_UNUSED(id);
    CORE_UNUSED(value);
    /*
    int retL = -1;
    int retR = -1;
    switch (propID)
    {
    case (ImageCaptureInterface::CAP_PROP_WIDTH):
        {
            if (captureLeft)
                retL = cvSetCaptureProperty(captureLeft, CV_CAP_PROP_FRAME_WIDTH, propValue);
            if (captureRight)
                retR = cvSetCaptureProperty(captureRight, CV_CAP_PROP_FRAME_WIDTH, propValue);
            break;
        }
    case (ImageCaptureInterface::CAP_PROP_HEIGHT):
        {
            if (captureLeft)
                retL = cvSetCaptureProperty(captureLeft, CV_CAP_PROP_FRAME_HEIGHT, propValue);
            if (captureRight)
                retR = cvSetCaptureProperty(captureRight, CV_CAP_PROP_FRAME_HEIGHT, propValue);
            break;
        }

    default:
        return ImageCaptureInterface::CAP_FAILURE;
    }

    return (ImageCaptureInterface::CapErrorCode) ((bool) retL + (bool) retR);
    */
    return ImageCaptureInterface::FAILURE;
}

ImageCaptureInterface::CapErrorCode OpenCVCaptureInterface::initCapture()
{
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode OpenCVCaptureInterface::startCapture()
{
    int retL = -1;
    int retR = -1;

    if (captureLeft)
    {
        retL = !cvSetCaptureProperty(captureLeft, CV_CAP_PROP_FRAME_WIDTH, 800);
        // retL |= cvSetCaptureProperty(captureLeft, CV_CAP_PROP_FRAME_HEIGHT, 600);
    }

    if (captureRight)
    {
        retR = !cvSetCaptureProperty(captureRight, CV_CAP_PROP_FRAME_WIDTH, 800);
        //retR |= cvSetCaptureProperty(captureRight, CV_CAP_PROP_FRAME_HEIGHT, 600);
    }

    qDebug("Setting 800x600: retL == %d, retR == %d\n", retL, retR);

    spin.start();

    return (ImageCaptureInterface::CapErrorCode) ((bool) retL + (bool) retR);
}

OpenCVCaptureInterface::~OpenCVCaptureInterface()
{
    spin.stop();
    spin.wait();
    cvReleaseCapture(&captureLeft);
    if (captureRight)
        cvReleaseCapture(&captureRight);
}
