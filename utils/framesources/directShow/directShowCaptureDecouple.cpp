#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QDebug>

#include "global.h"
#include "mathUtils.h" // roundDivUp
#include "preciseTimer.h"
#include "directShowCaptureDecouple.h"
#include "mjpegDecoderLazy.h"
#include "cameraControlParameters.h"

#include "decoupleYUYV.h"


DirectShowCaptureDecoupleInterface::DirectShowCaptureDecoupleInterface(string _devname)
{
    this->devname = _devname;

    //     Group Number                   1       2 3      4       56        7       8         9 10     11     1213    14
    QRegExp deviceStringPattern(QString("^([^,:]*)(:(\\d*)/(\\d*))?((:mjpeg)|(:yuyv)|(:fjpeg))?(:(\\d*)x(\\d*))?((:rc)|(:rc2)|(:sbs)|(:rcf))?$"));
    static const int DeviceGroup      = 1;
    static const int FpsNumGroup      = 3;
    static const int FpsDenumGroup    = 4;
    static const int CompressionGroup = 5;
    static const int WidthGroup       = 10;
    static const int HeightGroup      = 11;
    static const int CouplingGroup    = 12;


    printf ("Input string %s\n", _devname.c_str());
    QString qdevname(_devname.c_str());
    int result = deviceStringPattern.indexIn(qdevname);
    if (result == -1)
    {
        printf("Error in device string format:%s\n", _devname.c_str());
        return;
    }
    printf (
                "Parsed data:\n"
                "Device =%s\n"
                "FPS %s/%s\n"
                "Size [%sx%s]\n"
                "Compressing: %s\n"
                "Coupling: <%s>\n",
                deviceStringPattern.cap(DeviceGroup).toLatin1().constData(),
                deviceStringPattern.cap(FpsNumGroup).toLatin1().constData(),
                deviceStringPattern.cap(FpsDenumGroup).toLatin1().constData(),
                deviceStringPattern.cap(WidthGroup).toLatin1().constData(),
                deviceStringPattern.cap(HeightGroup).toLatin1().constData(),
                deviceStringPattern.cap(CompressionGroup).toLatin1().constData(),
                deviceStringPattern.cap(CouplingGroup).toLatin1().constData()
    );

    int cameraName =  deviceStringPattern.cap(DeviceGroup).toInt();

    bool err;
    int fpsnum = deviceStringPattern.cap(FpsNumGroup).toInt(&err);
    if (!err) fpsnum = 1;

    int fpsdenum = deviceStringPattern.cap(FpsDenumGroup).toInt(&err);
    if (!err) fpsdenum = 10;

    width = deviceStringPattern.cap(WidthGroup).toInt(&err);
    if (!err || width <= 0) width = 800;

    height = deviceStringPattern.cap(HeightGroup).toInt(&err);
    if (!err || height <= 0) height = 600;

    compressed = DirectShowCameraDescriptor::UNCOMPRESSED_YUV;
    int formatId = CAP_YUV;
    if (!deviceStringPattern.cap(CompressionGroup).isEmpty())
    {
       if        (!deviceStringPattern.cap(CompressionGroup).compare(QString(":rgb"))) {
           compressed = DirectShowCameraDescriptor::UNCOMPRESSED_RGB;
           formatId = CAP_RGB;
       } else if (!deviceStringPattern.cap(CompressionGroup).compare(QString(":mjpeg"))) {
           compressed = DirectShowCameraDescriptor::COMPRESSED_JPEG;
           formatId = CAP_MJPEG;
       } else if (!deviceStringPattern.cap(CompressionGroup).compare(QString(":fjpeg"))) {
           compressed = DirectShowCameraDescriptor::COMPRESSED_FAST_JPEG;
           formatId = CAP_MJPEG;
       }
    }


    coupling = DecoupleYUYV::ANAGLYPH_RC;
    if (!deviceStringPattern.cap(CouplingGroup).compare(QString(":sbs"))) {
        coupling = DecoupleYUYV::SIDEBYSIDE_STEREO;
    }
    if (!deviceStringPattern.cap(CouplingGroup).compare(QString(":rc2"))) {
        coupling = DecoupleYUYV::ANAGLYPH_RC_X2;
    }
    if (!deviceStringPattern.cap(CouplingGroup).compare(QString(":rcf"))) {
        coupling = DecoupleYUYV::ANAGLYPH_RC_FAST;
    }

    printf("Capture Right device: DShow %d\n", cameraName );
    printf("Format is: %s\n", DirectShowCameraDescriptor::codec_names[compressed]);
    printf("Coupling is: %d\n", coupling);


    /* TODO: Make cycle here */
    cameras[0].deviceHandle = DirectShowCapDll_initCapture(cameraName);

    CaptureTypeFormat format1 = {formatId, height, width, (int)((double)fpsdenum / fpsnum) }; // FixMe: probably round should be here?

    DirectShowCapDll_setFormat(cameras[0].deviceHandle, &format1);

    printf("Real Formats:\n");
    DirectShowCapDll_printSimpleFormat(&format1); printf("\n");
    height = format1.height;
    width  = format1.width;

    DirectShowCapDll_setFrameCallback(cameras[0].deviceHandle, this, DirectShowCaptureDecoupleInterface::callback);

    skippedCount = 0;
    isRunning = false;
}

void DirectShowCaptureDecoupleInterface::callback (void *thiz, DSCapDeviceId dev, FrameData data)
{
//    printf("Received new frame in a callback\n");
    DirectShowCaptureDecoupleInterface *pInterface = (DirectShowCaptureDecoupleInterface *)thiz;
    pInterface->memberCallback(dev, data);
}

ALIGN_STACK_SSE void DirectShowCaptureDecoupleInterface::memberCallback(DSCapDeviceId dev, FrameData data)
{
    CORE_UNUSED(dev);

//    SYNC_PRINT(("Received new frame in a member %d\n", dev));
    protectFrame.lock();

    DirectShowCameraDescriptor *camera = &cameras[0];

    PreciseTimer timer = PreciseTimer::currentTime();
    camera->gotBuffer = true;
    camera->timestamp = roundDivUp(data.timestamp, 10);

    delete camera->rawBuffer;
    camera->rawBuffer = new uint8_t[data.format.height * data.format.width * 2];

    if (data.format.type == CAP_YUV)
    {
        memcpy(camera->rawBuffer, data.data, data.format.height * data.format.width * 2);
//        SYNC_PRINT(("Got new frame %dx%d\n", data.format.height, data.format.width ));
    } else {
        memset(camera->rawBuffer, 0, data.format.height * data.format.width * 2);
    }

    camera->decodeTime = timer.usecsToNow();
    /* If both frames are in place */
    cameras[0].gotBuffer = false;

    CaptureStatistics stats;
    stats.values[CaptureStatistics::DESYNC_TIME] = 0;
    stats.values[CaptureStatistics::DECODING_TIME] = cameras[0].decodeTime;

    if (lastFrameTime.usecsTo(PreciseTimer()) != 0)
    {
        stats.values[CaptureStatistics::INTERFRAME_DELAY] = lastFrameTime.usecsToNow();
    }
    lastFrameTime = PreciseTimer::currentTime();

    frame_data_t frameData;
    frameData.timestamp = cameras[0].timestamp;
    newFrameReady(frameData);
    newStatisticsReady(stats);

    protectFrame.unlock();
}


ImageCaptureInterface::FramePair DirectShowCaptureDecoupleInterface::getFrame()
{
    protectFrame.lock();
    FramePair result(NULL, NULL);
    uint8_t *ptr = cameras[0].rawBuffer;

    DecoupleYUYV::decouple(height, width, ptr, coupling, result);

    result.leftTimeStamp = cameras[0].timestamp;
    result.rightTimeStamp = cameras[0].timestamp;
    protectFrame.unlock();
    return result;
}

ImageCaptureInterface::CapErrorCode DirectShowCaptureDecoupleInterface::initCapture()
{
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode DirectShowCaptureDecoupleInterface::startCapture()
{
    isRunning = true;
    DirectShowCapDll_start(cameras[0].deviceHandle);

    setCaptureProperty(CameraParameters::SATURATION, 0);
    //spin.start();
    return ImageCaptureInterface::SUCCESS;
}

DirectShowCaptureDecoupleInterface::~DirectShowCaptureDecoupleInterface()
{
    if (isRunning)
    {
        /*
            Callback is set safe and synchronously,
            so after the call of the callback reset, we will never again
            receive a frame. So it will be safe to destroy the object
        */

        DirectShowCapDll_setFrameCallback(cameras[0].deviceHandle, NULL, NULL);
        DirectShowCapDll_stop(cameras[0].deviceHandle);
    }
    isRunning = false;
}


ImageCaptureInterface::CapErrorCode DirectShowCaptureDecoupleInterface::queryCameraParameters(CameraParameters &parameters)
{
    cameras[0].queryCameraParameters(parameters);
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode DirectShowCaptureDecoupleInterface::setCaptureProperty(int id, int value)
{
    cameras[0].setCaptureProperty(id, value);
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode DirectShowCaptureDecoupleInterface::getCaptureProperty(int id, int *value)
{
    cameras[0].getCaptureProperty(id, value);
    return ImageCaptureInterface::SUCCESS;
}

int DirectShowCaptureDecoupleInterface::devicesNumber()
{
    int num = 0;
    DirectShowCapDll_devicesNumber(&num);
    return num;
}

ImageCaptureInterface::CapErrorCode DirectShowCaptureDecoupleInterface::getCaptureName(QString &value)
{
    char *name = NULL;
    DirectShowCapDll_deviceName(cameras[0].deviceHandle, &name);
    string str;
    int k = 0;
    while (name[k] != '\0')
    {
        str += name[k];
        k += 2;
    }
    value = strdup(str.c_str());
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode DirectShowCaptureDecoupleInterface::getFormats(int *num, CameraFormat *&format)
{
    DirectShowCapDll_getFormatNumber(cameras[0].deviceHandle, num);

    int number = *num;
    CaptureTypeFormat* captureTypeFormats = new CaptureTypeFormat[number];
    delete format;
    format = new CameraFormat[number];
    DirectShowCapDll_getFormats(cameras[0].deviceHandle, number, captureTypeFormats);
    for (int i = 0; i < number; i ++)
    {
        format[i].fps    = captureTypeFormats[i].fps;
        format[i].width  = captureTypeFormats[i].width;
        format[i].height = captureTypeFormats[i].height;
    }

    delete[] captureTypeFormats;

    return ImageCaptureInterface::SUCCESS;
}
