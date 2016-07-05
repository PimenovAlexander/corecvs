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
#include <opencv2/highgui/highgui.hpp>  // CvCapture
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <string>

#include "imageCaptureInterface.h"

#define CAP_ANY         0
#define CAP_VFW       200
#define CAP_DS        700

#define CAP_DEFAULT_DELAY 35

#ifdef interface            // msvc could define it as a "struct" at its header, but it's not a c++ keyword!
#undef interface
#endif

 class OpenCVCaptureInterface : public ImageCaptureInterface
 {
 private:

    /* This is a private helper class */
    class SpinThread : public QThread
    {
    public:
        SpinThread(OpenCVCaptureInterface *interface)
            : mInterface(interface), mStopping(false)
        {}

        virtual void run (void);
        void stop();
    private:
        OpenCVCaptureInterface *mInterface;
        bool mStopping;
    };

 public:
    SpinThread spin;
    FramePair  current;
    QMutex     protectFrame;
    CvCapture *captureLeft, *captureRight;

    /**
     * Delay to sleep before querying OpenCV for a new frame in milliseconds
     */
    unsigned int delay;


    OpenCVCaptureInterface(string _devname, unsigned int mode);

    virtual FramePair getFrame() override;

    virtual CapErrorCode setCaptureProperty(int id, int value) override;

    virtual CapErrorCode initCapture() override;
    virtual CapErrorCode startCapture() override;

    virtual ~OpenCVCaptureInterface();
 };

 #endif /* OPENCVCAPTURE_H_ */
