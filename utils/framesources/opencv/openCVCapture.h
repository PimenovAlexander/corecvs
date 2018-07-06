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
#include <string>

#include <QtCore/QThread>
#include <QtCore/QMutex>

#include "core/utils/global.h"

#include "core/framesources/imageCaptureInterface.h"

/*
#define CAP_ANY       (uint)  0
#define CAP_VFW       (uint)200
#define CAP_DS        (uint)700
*/

enum OpenCVCapType {
    OPENCV_CAP_ANY = 0,
    OPENCV_CAP_VFW = 200,
    OPENCV_CAP_DS  = 700
};

#define CAP_DEFAULT_DELAY 35

struct CvCapture;

class OpenCVCaptureInterface : public virtual ImageCaptureInterface
{
 private:

    /* This is a private helper class */
    class SpinThread : public QThread
    {
    public:
        SpinThread(OpenCVCaptureInterface *_interface)
            : mInterface(_interface), mStopping(false)
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


class OpenCvCaptureProducer : public ImageCaptureInterfaceProducer
{
public:
    OpenCvCaptureProducer()
    {}

    virtual std::string getPrefix() override
    {
        return "opencv:";
    }

    virtual ImageCaptureInterface *produce(std::string &name, bool /*isRGB*/) override
    {
        return new OpenCVCaptureInterface(name, OPENCV_CAP_ANY);
    }
};


 #endif /* OPENCVCAPTURE_H_ */
