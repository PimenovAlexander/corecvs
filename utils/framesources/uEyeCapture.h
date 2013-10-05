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
#include <QImage>
#include <QDebug>

#include "global.h"

#include "uEyeCameraDescriptor.h"
#include "cameraControlParameters.h"
#include "imageCaptureInterface.h"
#include "preciseTimer.h"

using namespace std;


class UEyeCaptureInterface : public ImageCaptureInterface
{
public:
    UEyeCaptureInterface(string _devname);
    virtual ~UEyeCaptureInterface();

    virtual FramePair getFrame();

    //void convertFromYUYV(G12Buffer *output, uint16_t * input);

    virtual CapErrorCode setCaptureProperty(int id, int value);
    virtual CapErrorCode getCaptureProperty(int id, int *value);

    virtual CapErrorCode queryCameraParameters(CameraParameters &parameter) ;


    virtual CapErrorCode initCapture();
    virtual CapErrorCode startCapture();


    static int ueyeTrace(int);


    static const double EXPOSURE_SCALER = 10.0;
    static const double FPS_SCALER      = 100.0;

private:

    /* This is a private helper class */
    class SpinThread : public QThread
    {
    public:
        UEyeCaptureInterface *capInterface;

        SpinThread(UEyeCaptureInterface *_capInterface) :
            capInterface(_capInterface)
        {}

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

    void decodeData(UEyeCameraDescriptor *camera, BufferDescriptorType *buffer, G12Buffer **output);

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
