#pragma once

#include "directShow.h"
#include "decoupleYUYV.h"

class DirectShowCaptureDecoupleInterface : public virtual ImageCaptureInterface
{
public:
   typedef ImageCaptureInterface::CapErrorCode CapErrorCode;

   int compressed;
   DecoupleYUYV::ImageCouplingType coupling;

   /* Main fields */
   QMutex protectFrame;
   string devname;  /**< Stores the device name*/
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


   DirectShowCaptureDecoupleInterface(string _devname);
   virtual ~DirectShowCaptureDecoupleInterface();

   virtual FramePair getFrame();

   virtual CapErrorCode initCapture();
   virtual CapErrorCode startCapture();

   virtual CapErrorCode queryCameraParameters(CameraParameters &parameters);
   virtual CapErrorCode setCaptureProperty(int id, int value);
   virtual CapErrorCode getCaptureProperty(int id, int *value);
   virtual CapErrorCode getCaptureName(QString &value);
   virtual CapErrorCode getFormats(int *num, CameraFormat *&format);

   static int devicesNumber();

private:
   void setFromCameraParam(CaptureParameter &param, CameraParameter &camParam);
};
