/**
 * \file OpenCVCapture.h
 * \brief Capture camera video stream using OpenCV library
 *
 * \date Jul 15, 2010
 * \author sergeyle
 */

#ifndef OPENCVCAPTURE_H_
#define OPENCVCAPTURE_H_

/* GCC 4.6 needs this to compile*/
#include <cstddef>
#include <opencv/highgui.h>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <string>

#include "imageCaptureInterface.h"

#define CAP_ANY         0
#define CAP_VFW       200
#define CAP_DS        700

#define CAP_DEFAULT_DELAY 35

#ifdef interface
#undef interface
#endif

using namespace std;

 class OpenCVCaptureInterface : public ImageCaptureInterface
 {
 private:

    /* This is a private helper class */
    class SpinThread : public QThread
    {
    public:
        SpinThread(OpenCVCaptureInterface *interface)
            : mInterface(interface), mStopping(false)
        {
        }

        virtual void run (void);
        void stop();
    private:
        OpenCVCaptureInterface *mInterface;
        bool mStopping;
    };

 public:
    SpinThread spin;
    FramePair current;
    QMutex protectFrame;

    /**
     * Delay to sleep before querying OpenCV for a new frame in milliseconds
     *
     */

    unsigned int delay;

    CvCapture* captureLeft, *captureRight;

    OpenCVCaptureInterface(string _devname, unsigned int mode);

    virtual FramePair getFrame();

    virtual CapErrorCode setCaptureProperty( int id, int value );

    virtual CapErrorCode initCapture();
    virtual CapErrorCode startCapture();

    virtual ~OpenCVCaptureInterface();
 };

 #endif /* OPENCVCAPTURE_H_ */
