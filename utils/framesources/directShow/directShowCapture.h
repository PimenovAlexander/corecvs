/**
 * \file directShowCapture.h
 * \brief Capture camera video stream using OpenCV library
 *
 * \date Jul 15, 2010
 * \author sergeyle
 */

#ifndef _DIRECT_SHOW_CVCAPTURE_H_
#define _DIRECT_SHOW_CVCAPTURE_H_

#include <stdint.h>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <string>

#include "directShow.h"
#include "imageCaptureInterface.h"
#include "preciseTimer.h"

#include "cameraControlParameters.h"

using namespace std;

 class DirectShowCaptureInterface : public ImageCaptureInterface
 {
 public:
    typedef ImageCaptureInterface::CapErrorCode CapErrorCode;

    static const char* codec_names[];
    int compressed;

    /* Main fields */
    QMutex protectFrame;
    string devname;  /**< Stores the device name*/
    DirectShowCameraDescriptor cameras[2];

    /* Statistics fields */
    PreciseTimer lastFrameTime;
    int skippedCount;
    int isRunning;

    /* Callback function */
    static void callback (void *thiz, DSCapDeviceId dev, FrameData data);
    void memberCallback(DSCapDeviceId dev, FrameData data);

    /* Maximum allowed desync */
    unsigned int delay;
    DirectShowCaptureInterface(string _devname);
    virtual FramePair getFrame();

    virtual CapErrorCode initCapture();
    virtual CapErrorCode startCapture();

    virtual CapErrorCode queryCameraParameters(CameraParameters &parameters);
    virtual CapErrorCode setCaptureProperty(int id, int value);
    virtual CapErrorCode getCaptureProperty(int id, int *value);
    virtual CapErrorCode getCaptureName(QString &value);
    virtual CapErrorCode getFormats(int *num, CameraFormat *&format);
    virtual CapErrorCode getDeviceName(int num, QString &name);

    virtual ~DirectShowCaptureInterface();

 private:
    bool isCorrectDeviceHandle(int cameraNum);
 };

#endif /* _DIRECT_SHOW_CVCAPTURE_H_ */

