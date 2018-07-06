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
#include <string>
#include <mutex>

#include "directShow.h"
#include "core/framesources/imageCaptureInterface.h"
#include "core/framesources/cameraControlParameters.h"    // CameraParameters
#include "core/utils/preciseTimer.h"
#include "../../frames.h"     // Frames::

#define PREFFERED_RGB_BPP 24
#define AUTOSELECT_FORMAT_FEATURE -255


 class DirectShowCaptureInterface : public virtual ImageCaptureInterface
 {
 public:
    typedef ImageCaptureInterface::CapErrorCode CapErrorCode;

    /* Main fields */
    std::mutex                  mProtectFrame;
    string                      mDevname;  /**< Stores the device name*/
    DirectShowCameraDescriptor  mCameras  [MAX_INPUTS_NUMBER];
    CaptureTypeFormat           mFormats  [MAX_INPUTS_NUMBER];
    int                         mDeviceIDs[MAX_INPUTS_NUMBER];
    int                         mCompressed;

    /* Statistics fields */
    PreciseTimer                lastFrameTime;
    int                         skippedCount;
    int                         isRunning;

    bool                        mAutoFormat;

    /* Maximum allowed desync */
    //unsigned int                delay;

    DirectShowCaptureInterface(const string &devname, bool isRgb = false);
    DirectShowCaptureInterface(const string &devname, int h, int w, int fps, bool isRgb);
    DirectShowCaptureInterface(const string &devname, ImageCaptureInterface::CameraFormat format, bool isRgb);

    virtual FramePair    getFrame() override;
    virtual FramePair    getFrameRGB24() override;

    virtual CapErrorCode initCapture() override;
    virtual CapErrorCode startCapture() override;

    virtual bool         getCurrentFormat(CameraFormat &format);

    virtual CapErrorCode queryCameraParameters(CameraParameters &parameters) override;
    virtual CapErrorCode setCaptureProperty(int id, int value) override;
    virtual CapErrorCode getCaptureProperty(int id, int *value) override;
    virtual CapErrorCode getCaptureName(std::string &value)  override;
    virtual CapErrorCode getFormats(int *num, CameraFormat *&formats) override;
    virtual std::string  getInterfaceName()  override;
    virtual CapErrorCode getDeviceName(int num, std::string &name)  override;
    virtual std::string  getDeviceSerial(int num = Frames::LEFT_FRAME)  override;


    /* Callback function */
    static void callback (void *thiz, DSCapDeviceId dev, FrameData data);
    void memberCallback(DSCapDeviceId dev, FrameData data);

    static void getAllCameras(vector<std::string> &cameras);

    virtual ~DirectShowCaptureInterface();

 private:
    void         init(const string &devname, int h, int w, int fps, bool isRgb, int bpp, int compressed);
    void         initForAutoFormat(const string &devname, int h, int w, int fps, bool isRgb);

    bool         isCorrectDeviceHandle(int cameraNum);
    CapErrorCode getCaptureFormats(int &number, CaptureTypeFormat *&list);
    CapErrorCode getCameraFormatsForResolution(int h, int w, std::vector<CAPTURE_FORMAT_TYPE> &formats);
    int          selectCameraFormat(int h, int w);
 };

#endif /* _DIRECT_SHOW_CVCAPTURE_H_ */
