/**
 * \file V4L2Capture.h
 * \brief Add Comment Here
 *
 * \date Apr 9, 2010
 * \author alexander
 */

#pragma once

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <string>
#include <errno.h>
#include <linux/videodev2.h>

#include "global.h"

#include "V4L2.h"
#include "rgb24Buffer.h"
#include "cameraControlParameters.h"
#include "imageCaptureInterface.h"
#include "preciseTimer.h"
#include "../../frames.h"

using namespace std;



class V4L2CaptureInterface : public ImageCaptureInterface
{
public:
    V4L2CaptureInterface(string _devname, bool isRgb = false);
    virtual ~V4L2CaptureInterface();

    virtual int setConfigurationString(string _devname);

    virtual FramePair getFrame();
    virtual FramePair getFrameRGB24();

    virtual CapErrorCode setCaptureProperty(int id, int value);
    virtual CapErrorCode getCaptureProperty(int id, int *value);

    virtual CapErrorCode queryCameraParameters(CameraParameters &parameter) ;


    virtual CapErrorCode initCapture();
    virtual CapErrorCode startCapture();

    virtual CapErrorCode getCaptureName(QString &value);
    virtual CapErrorCode getFormats(int *num, CameraFormat *& formats);

    virtual QString      getInterfaceName();
    virtual CapErrorCode getDeviceName(int num, QString &name);

    static void getAllCameras(int *num, int *&cameras);

    enum DecoderType{
        UNCOMPRESSED,
        COMPRESSED_JPEG,
        COMPRESSED_FAST_JPEG,
        CODEC_NUMBER
    };

    static const char* CODEC_NAMES[];
private:

    static const unsigned LoopQueueBufferNumber = 8;
    static const unsigned LoopQueueBufferNumberMin = 5;

    string interfaceName;  /**< Stores the device name*/

    /* This is a private helper class that decribes the thread that will block waiting for new frame*/
    class SpinThread : public QThread
    {
    public:
        V4L2CaptureInterface *interface;

        SpinThread(V4L2CaptureInterface *_interface) :
            interface(_interface)
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
    void decodeDataRGB24(V4L2CameraDescriptor *camera, V4L2BufferDescriptor *buffer, RGB24Buffer **output);



    string           deviceName[Frames::MAX_INPUTS_NUMBER];
    V4L2CameraMode   cameraMode;
    V4L2CameraDescriptor camera[Frames::MAX_INPUTS_NUMBER];

    int formatH;
    int formatW;

    DecoderType decoder;

    SpinThread spin;      /**< Spin thread that blocks waiting for the frames */
    QMutex protectFrame;  /**< This mutex protects both buffers from concurrent reading/writing
                            *  If somebody is reading of modifying the buffer it should lock the mutex
                            **/
    QMutex spinRunning;
    volatile bool shouldStopSpinThread;

    V4L2BufferDescriptor currentFrame[Frames::MAX_INPUTS_NUMBER];

    /* Statistics fields */
    PreciseTimer lastFrameTime;
    uint64_t frameDelay;

    uint64_t skippedCount;

    uint64_t maxDesync; /**< Maximum de-synchronization that we will not attempt to correct */

    bool mIsRgb;

};
