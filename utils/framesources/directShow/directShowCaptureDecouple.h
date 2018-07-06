#pragma once

#include <string>
#include <mutex>

#include "core/utils/global.h"
#include "core/utils/preciseTimer.h"
#include "core/framesources/imageCaptureInterface.h"
#include "core/framesources/decoders/decoupleYUYV.h"
#include "directShow.h"

class DirectShowCaptureDecoupleInterface : public virtual ImageCaptureInterface
{
public:
   typedef ImageCaptureInterface::CapErrorCode CapErrorCode;

   int compressed;
   DecoupleYUYV::ImageCouplingType coupling;

   /* Main fields */
   std::mutex  protectFrame;
   std::string devname;  /**< Stores the device name*/
   DirectShowCameraDescriptor cameras[2];

   int width;
   int height;

   /* Statistics fields */
   PreciseTimer lastFrameTime;
   int skippedCount;
   int isRunning;

   /* Maximum allowed desync */
   unsigned int delay;

   /* Callback function */
   static void callback (void *thiz, DSCapDeviceId dev, FrameData data);
   void memberCallback(DSCapDeviceId dev, FrameData data);

   DirectShowCaptureDecoupleInterface(std::string _devname);
   virtual ~DirectShowCaptureDecoupleInterface();

   virtual FramePair getFrame();

   virtual CapErrorCode initCapture();
   virtual CapErrorCode startCapture();

   virtual CapErrorCode queryCameraParameters(CameraParameters &parameters) override;
   virtual CapErrorCode setCaptureProperty(int id, int value) override;
   virtual CapErrorCode getCaptureProperty(int id, int *value) override;
   virtual CapErrorCode getCaptureName(std::string &value) override;
   virtual CapErrorCode getFormats(int *num, CameraFormat *&format) override;

   static int devicesNumber();

private:
   //void setFromCameraParam(CaptureParameter &param, CameraParameter &camParam);
};
