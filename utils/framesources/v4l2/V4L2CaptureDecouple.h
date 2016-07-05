#ifndef V4L2CAPTUREDECOUPLE_H_
#define V4L2CAPTUREDECOUPLE_H_
/*
 * V4L2CaptureDecouple.h
 *
 *  Created on: Jul 21, 2012
 *      Author: alexander
 */

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <string>

#include "global.h"

#include "preciseTimer.h"
#include "imageCaptureInterface.h"
#include "cameraControlParameters.h"
#include "decoupleYUYV.h"
#include "V4L2Capture.h"
#include "V4L2.h"

using std::string;


class V4L2CaptureDecoupleInterface : public ImageCaptureInterface
{
public:
    V4L2CaptureDecoupleInterface(string _devname);
    virtual ~V4L2CaptureDecoupleInterface();

    virtual FramePair getFrame() override;

    virtual CapErrorCode setCaptureProperty(int id, int value) override;
    virtual CapErrorCode getCaptureProperty(int id, int *value) override;

    virtual CapErrorCode queryCameraParameters(CameraParameters &parameter) override;

    virtual QString getInterfaceName() override;

    virtual CapErrorCode initCapture() override;
    virtual CapErrorCode startCapture() override;

private:
    static const unsigned LoopQueueBufferNumber = 8;
    static const unsigned LoopQueueBufferNumberMin = 5;

    string devname;  /**< Stores the device name*/

    /* This is a private helper class that decribes the thread that will block waiting for new frame*/
    class SpinThread : public QThread
    {
    public:
        V4L2CaptureDecoupleInterface *mInterface;

        SpinThread(V4L2CaptureDecoupleInterface *interface) :
            mInterface(interface)
        {}

        virtual void run (void);
    };

    int getParameter(int deviceHandle, int type, char const * str, CaptureParameter &param
                     , ImageCaptureInterface::CapPropertyAccessType &accessFlags);

    void setCaptureDeviceParameters(const int handle, const int prop, const int32_t val
                    , int &res, const char* text) const;

    int queryCaptureDeviceParmeter(const int handle, const uint32_t propID, const char *text
                    , ImageCaptureInterface::CapPropertyAccessType &accessFlags, v4l2_queryctrl &request) const;

    V4L2CameraDescriptor initCamera(string camFileHandle, int h, int w, int fpsnum = 1, int fpsdenum = 10);

    int start(V4L2CameraDescriptor *cam);
    int initBuffers(V4L2CameraDescriptor *cam);
    V4L2BufferDescriptor dequeue(V4L2CameraDescriptor *cam);
    void enqueue(V4L2CameraDescriptor *cam, V4L2BufferDescriptor buffer);

    uint16_t *decodeMjpeg(unsigned char *buffer);
    void decodeData(V4L2CameraDescriptor *camera, V4L2BufferDescriptor *buffer, G12Buffer **output);

    string           deviceName;
    V4L2CameraMode   cameraMode;
    V4L2CameraDescriptor camera;

    V4L2CaptureInterface::DecoderType decoder;
    DecoupleYUYV::ImageCouplingType coupling;

    unsigned formatH;
    unsigned formatW;

    SpinThread spin;      /**< Spin thread that blocks waiting for the frames */
    QMutex protectFrame;  /**< This mutex protects both buffers from concurent reading/writind
                            *  If somebody is reading of modifying the buffer it should lok the mutex
                            **/
    QMutex spinRunning;
    volatile bool shouldStopSpinThread;

    V4L2BufferDescriptor current;

    /* Statistics fields */
    PreciseTimer lastFrameTime;
    uint64_t frameDelay;

    uint64_t skippedCount;

};



#endif /* V4L2CAPTUREDECOUPLE_H_ */
