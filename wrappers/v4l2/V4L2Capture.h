#pragma once
/**
 * \file V4L2Capture.h
 * \brief Add Comment Here
 *
 * \date Apr 9, 2010
 * \author alexander
 */
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <string>
#include <errno.h>
#include <linux/videodev2.h>

#include "core/utils/global.h"

#include "V4L2.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/framesources/cameraControlParameters.h"
#include "core/framesources/imageCaptureInterface.h"
#include "core/utils/preciseTimer.h"
#include "core/framesources/frames.h"

using namespace std;


class V4L2CaptureInterface : public virtual ImageCaptureInterface
{
public:
    V4L2CaptureInterface(string _devname, bool isRgb = false);
    V4L2CaptureInterface(string _devname, int h, int w, int fps, bool isRgb = false);
    V4L2CaptureInterface(string _devname, CameraFormat format, bool isRgb = false);

    virtual ~V4L2CaptureInterface();

    virtual int setConfigurationString(string _devname);

    virtual FramePair getFrame() override;
    virtual FramePair getFrameRGB24() override;

    virtual CapErrorCode setCaptureProperty(int id, int value) override;
    virtual CapErrorCode getCaptureProperty(int id, int *value) override;

    virtual CapErrorCode queryCameraParameters(CameraParameters &parameter) override;


    virtual CapErrorCode initCapture() override;
    virtual CapErrorCode startCapture() override;

    virtual CapErrorCode getCaptureName(std::string &value) override;
    virtual CapErrorCode getFormats(int *num, CameraFormat *& formats) override;

    virtual std::string  getInterfaceName() override;
    virtual CapErrorCode getDeviceName(int num, std::string &name) override;

    virtual std::string  getDeviceSerial(int num = LEFT_FRAME) override;

    static void getAllCameras(int *num, int *&cameras);

    static const int MAX_SCAN_DEVICE_ID = 20;
    static void getAllCameras(vector<std::string> &cameras, int maxDeviceId = MAX_SCAN_DEVICE_ID);



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
        V4L2CaptureInterface *mInterface;

        SpinThread(V4L2CaptureInterface *interface) :
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
    void decodeDataRGB24(V4L2CameraDescriptor *camera, V4L2BufferDescriptor *buffer, RGB24Buffer **output);



    string           deviceName[MAX_INPUTS_NUMBER];
    V4L2CameraMode   cameraMode;
    V4L2CameraDescriptor camera[MAX_INPUTS_NUMBER];

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
};


class V4L2CaptureInterfaceProducer : public ImageCaptureInterfaceProducer
{
public:
    V4L2CaptureInterfaceProducer()
    {}

    virtual std::string getPrefix() override
    {
        return "v4l2:";
    }

    virtual ImageCaptureInterface *produce(std::string &name, bool isRGB) override
    {
        return new V4L2CaptureInterface(name, isRGB);
    }
};
