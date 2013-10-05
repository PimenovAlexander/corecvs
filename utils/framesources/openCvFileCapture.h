#pragma once
/**
 * \brief Capture video stream from avi file using OpenCV library
 */
#include <cstddef>  // GCC 4.6 needs this to compile
#include <opencv/highgui.h>

#include "abstractFileCapture.h"

class OpenCvFileCapture : public AbstractFileCapture
{
public:
    OpenCvFileCapture(QString const &params);

    virtual ImageCaptureInterface::CapErrorCode startCapture();
    virtual FramePair getFrame();

private:
    class SpinThread : public AbstractFileCaptureSpinThread
    {
    public:
        SpinThread(AbstractFileCapture *pInterface
                   , int delay
                   , ImageCaptureInterface::FramePair &framePair
                   , cv::VideoCapture &capture
                   , string const &path);

        virtual bool grabFramePair();

    private:
        static cint maxNumberOfTries = 5;

        cv::VideoCapture &mCapture;
        string const     &mPath;
        int               mTry;
    };

    cv::VideoCapture mCapture;
};
