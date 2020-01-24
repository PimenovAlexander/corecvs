#include "imageCaptureInterfaceQt.h"

#include "core/framesources/file/fileCapture.h"
#include "core/framesources/file/precCapture.h"

#ifdef WITH_V4L2
# include "V4L2Capture.h"
# include "V4L2CaptureDecouple.h"
#endif

#ifdef WITH_DIRECTSHOW
# include "directShowCapture.h"
# include "directShowCaptureDecouple.h"
#endif

#ifdef WITH_UEYE
# include "uEyeCapture.h"
#endif

#ifdef WITH_FLYCAP
# include "flyCapCapture.h"
#endif

#ifdef WITH_SYNCCAM
#include "syncCamerasCaptureInterface.h"
#endif

#ifdef WITH_AVCODEC
#include "aviCapture.h"
#include "rtspCapture.h"
#endif

#ifdef WITH_OPENCV
 #include "opencv/openCVCapture.h"
 #include "opencv/openCVFileCapture.h"
#endif

ImageCaptureInterfaceQt* ImageCaptureInterfaceQtFactory::fabric(string input, bool isRGB)
{
    SYNC_PRINT(("ImageCaptureInterfaceQtFactory::fabric(%s, rgb=%s):called\n", input.c_str(), isRGB ? "true" : "false"));


    string file("file:");
    if (input.substr(0, file.size()).compare(file) == 0)
    {
        string tmp = input.substr(file.size());
        return new ImageCaptureInterfaceWrapper<FileCaptureInterface>(tmp);
    }

    string prec("prec:");
    if (input.substr(0, prec.size()).compare(prec) == 0)
    {
        string tmp = input.substr(prec.size());
        return new ImageCaptureInterfaceWrapper<FilePreciseCapture>(tmp, true, isRGB);
    }

#ifdef WITH_SYNCCAM
    string sync("sync:");
    if (input.substr(0, sync.size()).compare(sync) == 0)
    {
        string tmp = input.substr(sync.size());
        return new SyncCamerasCaptureInterface(tmp);
    }
#endif

#ifdef WITH_V4L2
    string v4l2("v4l2:");
    if (input.substr(0, v4l2.size()).compare(v4l2) == 0)
    {
        string tmp = input.substr(v4l2.size());
        return new ImageCaptureInterfaceWrapper<V4L2CaptureInterface>(tmp, isRGB);
    }

    string v4l2d("v4l2d:");
    if (input.substr(0, v4l2d.size()).compare(v4l2d) == 0)
    {
        string tmp = input.substr(v4l2d.size());
        return new ImageCaptureInterfaceWrapper<V4L2CaptureDecoupleInterface>(tmp);
    }
#endif

#ifdef WITH_UEYE
    string ueye("ueye:");
    if (input.substr(0, ueye.size()).compare(ueye) == 0)
    {
        string tmp = input.substr(ueye.size());
        return new ImageCaptureInterfaceWrapper<UEyeCaptureInterface>(tmp);
    }
#endif

#ifdef WITH_FLYCAP
    string flycap("flycap:");
    if (input.substr(0, flycap.size()).compare(flycap) == 0)
    {
        string tmp = input.substr(flycap.size());
        return new ImageCaptureInterfaceWrapper<FlyCapCaptureInterface>(tmp);
    }
#endif

#ifdef WITH_DIRECTSHOW
    string dshow("dshow:");
    if (input.substr(0, dshow.size()).compare(dshow) == 0)
    {
        string tmp = input.substr(dshow.size());
        return new ImageCaptureInterfaceWrapper<DirectShowCaptureInterface>(tmp, isRGB);
    }

    string dshowd("dshowd:");
    if (input.substr(0, dshowd.size()).compare(dshowd) == 0)
    {
        string tmp = input.substr(dshowd.size());
        return new ImageCaptureInterfaceWrapper<DirectShowCaptureDecoupleInterface>(tmp);
    }
#endif

#ifdef WITH_AVCODEC
    string avcodec("avcodec:");
    if (input.substr(0, avcodec.size()).compare(avcodec) == 0)
    {
        SYNC_PRINT(("ImageCaptureInterface::fabric(): Creating avcodec input"));
        string tmp = input.substr(avcodec.size());
        return new ImageCaptureInterfaceWrapper<AviCapture>(tmp);
    }

    string rtsp("rtsp:");
    if (input.substr(0, rtsp.size()).compare(rtsp) == 0)
    {
        SYNC_PRINT(("ImageCaptureInterface::fabric(): Creating avcodec input"));
        string tmp = input.substr(rtsp.size());
        return new ImageCaptureInterfaceWrapper<RTSPCapture>(tmp);
    }
#endif

#ifdef WITH_OPENCV
    string any("any:");
    if (input.substr(0, any.size()).compare(any) == 0)
    {
        string tmp = input.substr(any.size());
        return new ImageCaptureInterfaceWrapper<OpenCVCaptureInterface>(tmp, (uint)OPENCV_CAP_ANY);
    }

    string vfw("vfw:");
    if (input.substr(0, vfw.size()).compare(vfw) == 0)
    {
        string tmp = input.substr(vfw.size());
        return new ImageCaptureInterfaceWrapper<OpenCVCaptureInterface>(tmp, (uint)OPENCV_CAP_VFW);
    }

    string ds("ds:");
    if (input.substr(0, ds.size()).compare(ds) == 0)
    {
        string tmp = input.substr(ds.size());
        return new ImageCaptureInterfaceWrapper<OpenCVCaptureInterface>(tmp, (uint)OPENCV_CAP_DS);
    }

    string opencv_file("opencv_file:");
    if (input.substr(0, opencv_file.size()).compare(opencv_file) == 0)
    {
        string tmp = input.substr(opencv_file.size());
        return new ImageCaptureInterfaceWrapper<OpenCvFileCapture>(tmp);
    }
#endif

    return NULL;
}

ImageCaptureInterfaceQt *ImageCaptureInterfaceQtFactory::fabric(string input, int h, int w, int fps, bool isRgb)
{
#ifdef WITH_V4L2
    string v4l2("v4l2:");
    if (input.substr(0, v4l2.size()).compare(v4l2) == 0)
    {
        string tmp = input.substr(v4l2.size());
        return new ImageCaptureInterfaceWrapper<V4L2CaptureInterface>(tmp, h, w, fps, isRgb);
    }
#endif

#ifdef WITH_UEYE
    string ueye("ueye:");
    if (input.substr(0, ueye.size()).compare(ueye) == 0)
    {
        string tmp = input.substr(ueye.size());
        return new ImageCaptureInterfaceWrapper<UEyeCaptureInterface>(tmp, h, w, fps, isRgb);
    }
#endif

#ifdef WITH_DIRECTSHOW
    string dshow("dshow:");
    if (input.substr(0, dshow.size()).compare(dshow) == 0)
    {
        string tmp = input.substr(dshow.size());
        return new ImageCaptureInterfaceWrapper<DirectShowCaptureInterface>(tmp, h, w, fps, isRgb);
    }
#endif

    return NULL;
}
