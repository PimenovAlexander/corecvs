/**
 * \file OpenCVCapture.cpp
 * \brief Capture camera video stream using OpenCV library
 *
 * \date Jul 15, 2010
 * \author sergeyle
 */
#include <QtCore/QRegExp>
#include <QtCore/QString>

#include <opencv2/highgui/highgui.hpp> // cvCaptureFromCAM

#include "core/utils/global.h"

#include "openCVCapture.h"
#include "openCVHelper.h"

OpenCVCaptureInterface::OpenCVCaptureInterface(string _devname,  unsigned int mode) :
     spin(this)
   , captureLeft(NULL)
   , captureRight(NULL)
{
    unsigned int domain = 0;

    if      (mode == CAP_ANY)
    {
        domain = CV_CAP_ANY;
        printf("OpenCVCaptureInterface(): using ANY. Input: %s\n", _devname.c_str());
        fflush(stdout);
    }
    else if (mode == CAP_VFW)
    {
        domain = CV_CAP_VFW;
        printf("OpenCVCaptureInterface(): using VFW. Input: %s\n", _devname.c_str());
        fflush(stdout);
    }
    else if (mode == CAP_DS)
    {
        domain = CV_CAP_DSHOW;
        printf("OpenCVCaptureInterface(): using DirectShow. Input: %s\n", _devname.c_str());
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

    captureLeft  = NULL;
    captureRight = NULL;

    int id1 = left_index  + domain;
    int id2 = right_index + domain;

    SYNC_PRINT(("OpenCVCaptureInterface(): Calling first  cvCaptureFromCAM(%d)\n", id1));
    captureLeft = cvCaptureFromCAM(id1);
    SYNC_PRINT(("OpenCVCaptureInterface(): result is 0x%p\n", (void *)captureLeft));

    SYNC_PRINT(("OpenCVCaptureInterface(): Calling second cvCaptureFromCAM(%d)\n", id2));
    captureRight = cvCaptureFromCAM(id2);
    SYNC_PRINT(("OpenCVCaptureInterface(): result is 0x%p\n", (void *)captureRight));

    if (captureLeft == NULL && captureRight == NULL)
    {
        printf("OpenCVCaptureInterface(): Both cameras failed to initialize");
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    // We assume that if there is only one camera active, it is the left camera
    if (captureLeft == NULL && captureRight != NULL)
    {
        captureLeft  = captureRight;
        captureRight = NULL;
    }

    delay = devStringPattern.cap(DELAY_GROUP).toInt(&err);
    if (!err) {
        delay = CAP_DEFAULT_DELAY;
    }

    current.allocBuffers(800, 600);
}

void OpenCVCaptureInterface::SpinThread::run()
{    
    //SYNC_PRINT(("OpenCVCaptureInterface::SpinThread::run(): starting\n"));
    // We must capture a frame before getting properties.
    cvQueryFrame(mInterface->captureLeft);

    static int count = 0;

    while (!mStopping)
    {
        uint width  = cvGetCaptureProperty(mInterface->captureLeft, CV_CAP_PROP_FRAME_WIDTH);
        uint height = cvGetCaptureProperty(mInterface->captureLeft, CV_CAP_PROP_FRAME_HEIGHT);

        OpenCVCaptureInterface::FramePair *pair = &(mInterface->current);

        mInterface->protectFrame.lock();
            pair->allocBuffers(height, width);

            // OpenCV does not set timestamps for the frames
            pair->setTimeStampLeft (count * 10);
            pair->setTimeStampRight(count * 10);
            count++;

            if (mInterface->captureLeft != NULL)
            {
                OpenCvHelper::captureImageCopyToBuffer(mInterface->captureLeft, pair->bufferLeft());
            }
            if (mInterface->captureRight != NULL)
            {
                OpenCvHelper::captureImageCopyToBuffer(mInterface->captureRight, pair->bufferRight());
            }
        mInterface->protectFrame.unlock();

        frame_data_t frameData;
        frameData.timestamp = pair->timeStamp();

        //SYNC_PRINT(("OpenCVCaptureInterface::SpinThread::run(): notifyAboutNewFrame()\n"));
        mInterface->notifyAboutNewFrame(frameData);

        /*
         * If we don't sleep here we'll find ourselves flooding the application with false new
         * frame notifications
         */
        if (mInterface->delay) {
            msleep(mInterface->delay);
        }
    }
}

void OpenCVCaptureInterface::SpinThread::stop()
{
    mStopping = true;
}

OpenCVCaptureInterface::FramePair OpenCVCaptureInterface::getFrame()
{
    SYNC_PRINT(("OpenCVCaptureInterface::SpinThread::getFrame(): called"));
    protectFrame.lock();
        FramePair result = current.clone();
    protectFrame.unlock();
    return result;
}

ImageCaptureInterface::CapErrorCode OpenCVCaptureInterface::setCaptureProperty(int id, int value)
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

    if (captureLeft != NULL)
    {
        retL = !cvSetCaptureProperty(captureLeft, CV_CAP_PROP_FRAME_WIDTH, 800);
        // retL |= cvSetCaptureProperty(captureLeft, CV_CAP_PROP_FRAME_HEIGHT, 600);
    }

    if (captureRight != NULL)
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

    if (captureRight) {
        cvReleaseCapture(&captureRight);
    }
}
