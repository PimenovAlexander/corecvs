/**
 * \file UEyeCapture.h
 * \brief Add Comment Here
 *
 * \date March 15, 2012
 * \author kat
 */

#pragma once

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <string>
#include <errno.h>
#include <uEye.h>

#include "core/utils/global.h"

#include "uEyeCameraDescriptor.h"
#include "core/framesources/cameraControlParameters.h"
#include "core/framesources/imageCaptureInterface.h"
#include "core/utils/preciseTimer.h"


class UEyeCaptureInterface : public virtual ImageCaptureInterface
{
public:
    UEyeCaptureInterface(string _devname);
    UEyeCaptureInterface(string _devname, int h, int w, int fps, bool isRgb = false);

    virtual int setConfigurationString(string _devname, bool isRgb = false);

    virtual ~UEyeCaptureInterface();

    virtual FramePair getFrame() override;
    virtual FramePair getFrameRGB24() override;

    //void convertFromYUYV(G12Buffer *output, uint16_t * input);

    virtual CapErrorCode setCaptureProperty(int id, int value) override;
    virtual CapErrorCode getCaptureProperty(int id, int *value) override;

    virtual CapErrorCode queryCameraParameters(CameraParameters &parameter) override;


    virtual CapErrorCode initCapture() override;
    virtual CapErrorCode startCapture() override;

    virtual CapErrorCode getFormats(int *num, CameraFormat *& formats) override;

    static void getAllCameras(vector<std::string> &cameras);
    virtual std::string  getDeviceSerial(int num = LEFT_FRAME) override;
    virtual std::string getInterfaceName()  override;

    static int ueyeTrace(int res, const char *prefix = NULL);


    static const double EXPOSURE_SCALER /*= 10.0*/;
    static const double FPS_SCALER      /*= 100.0*/;

private:
     string interfaceName;  /**< Stores the device name*/

    /* This is a private helper class */
    class SpinThread : public QThread
    {
    public:
        UEyeCaptureInterface *capInterface;

        SpinThread(UEyeCaptureInterface *_capInterface) :
            capInterface(_capInterface)
        {
            this->setObjectName("UEyeCaptureInterface:SpinThread");
        }

        void run (void);
    };

    string devname;  /**< Stores the device name*/

    enum SyncType {
        NO_SYNC,
        SOFT_SYNC,
        HARD_SYNC,
        FRAME_HARD_SYNC,
        EXT_SYNC
    };

    SyncType sync;

    void decodeData  (UEyeCameraDescriptor *camera, BufferDescriptorType *buffer, G12Buffer **output);
    void decodeData24(UEyeCameraDescriptor *camera, BufferDescriptorType *buffer, RGB24Buffer **output);

    UEyeCameraDescriptor leftCamera;
    UEyeCameraDescriptor rightCamera;

    BufferDescriptorType *currentLeft;
    BufferDescriptorType *currentRight;


    SpinThread spin;      /**< Spin thread that blocks waiting for the frames */
    QMutex protectFrame;  /**< This mutex protects both buffers from concurrent reading/writind
                            *  If somebody is reading of modifying the buffer it should look the mutex
                            **/
    QMutex spinRunning;
    volatile bool shouldStopSpinThread;

    /* Statistics fields */
    PreciseTimer lastFrameTime;
    uint64_t frameDelay;

    uint64_t skippedCount;
    uint64_t triggerSkippedCount;

    uint64_t maxDesync; /**< Maximum de-synchronization that we will not attempt to correct */

};
