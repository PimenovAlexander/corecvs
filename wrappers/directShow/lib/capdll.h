#ifndef _CAPDLL_H_
#define _CAPDLL_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CAPDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CAPDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef CAPDLL_EXPORTS
#  define CAPDLL_API  __declspec(dllexport)
#  ifdef _MSC_VER
#    pragma message ( "Exporting capdll funcs" )
#  endif
#else
#  define CAPDLL_API __declspec(dllimport)
#  ifdef _MSC_VER
#    pragma message ( "Importing capdll funcs" )
#  endif
#endif

extern "C" {

#define WRONG_DEVICE 1000


enum CAPTURE_FORMAT_TYPE {
    CAP_MJPEG,
    CAP_YUV,
    CAP_RGB,
    CAP_I420,
    CAP_UNKOWN
};


struct CameraParameter {
    long minValue;
    long maxValue;
    long value;
    long defaultValue;
    long step;
    bool isAuto;
};

enum CAPDLLControlProperty {

    CAPDLL_Control_First,
    CAPDLL_Control_Pan = CAPDLL_Control_First,
    CAPDLL_Control_Tilt,
    CAPDLL_Control_Roll,
    CAPDLL_Control_Zoom,
    CAPDLL_Control_Exposure,
    CAPDLL_Control_Iris,
    CAPDLL_Control_Focus,
    CAPDLL_Control_Last,

    CAPDLL_AMP_First,
    CAPDLL_AMP_Brightness = CAPDLL_AMP_First,
    CAPDLL_AMP_Contrast,
    CAPDLL_AMP_Hue,
    CAPDLL_AMP_Saturation,
    CAPDLL_AMP_Sharpness,
    CAPDLL_AMP_ColorEnable,
    CAPDLL_AMP_WhiteBalance,
    CAPDLL_AMP_BacklightCompensation,
    CAPDLL_AMP_Gain,
    CAPDLL_AMP_Last
};


struct CaptureTypeFormat {
    int type;
    int height;
    int width;
    int fps;
};

struct FrameData {
    CaptureTypeFormat format;    
    long long timestamp;
    long size;
    void *data;
};

typedef int  DSCapDeviceId;

typedef    void             FrameCallbackFunctor(void *ptr, DSCapDeviceId device, FrameData data);

CAPDLL_API void             DirectShowCapDll_devicesNumber(int *num);

CAPDLL_API void             DirectShowCapDll_printDeviceList(void);
CAPDLL_API void             DirectShowCapDll_printSimpleFormat(CaptureTypeFormat *format);

CAPDLL_API DSCapDeviceId    DirectShowCapDll_initCapture     (int deviceId);

CAPDLL_API int              DirectShowCapDll_deviceName(DSCapDeviceId device, char **name);

CAPDLL_API int              DirectShowCapDll_getFormatNumber (DSCapDeviceId device, int *num);
CAPDLL_API int              DirectShowCapDll_getFormats      (DSCapDeviceId device, int num, CaptureTypeFormat *formats);
CAPDLL_API int              DirectShowCapDll_setFormat       (DSCapDeviceId device, CaptureTypeFormat *format);

CAPDLL_API int              DirectShowCapDll_getCameraProp   (DSCapDeviceId device, CAPDLLControlProperty prop, CameraParameter *param);
CAPDLL_API int              DirectShowCapDll_setCameraProp   (DSCapDeviceId device, CAPDLLControlProperty prop, CameraParameter *param);


CAPDLL_API int              DirectShowCapDll_setFrameCallback(DSCapDeviceId device, void *ptr, FrameCallbackFunctor *callback);
CAPDLL_API int              DirectShowCapDll_start           (DSCapDeviceId device);
CAPDLL_API int              DirectShowCapDll_pause           (DSCapDeviceId device);
CAPDLL_API int              DirectShowCapDll_stop            (DSCapDeviceId device);
CAPDLL_API int              DirectShowCapDll_deinit          (DSCapDeviceId device);


} // extren "C"

#endif // _CAPDLL_H_
