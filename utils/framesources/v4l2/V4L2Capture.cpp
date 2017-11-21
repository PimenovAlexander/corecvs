/**
 * \file V4L2Capture.cpp
 * \brief Add Comment Here
 *
 * \date Apr 9, 2010
 * \author alexander
 */

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sstream>

#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QtCore>

#include "core/utils/global.h"

#include "V4L2Capture.h"
#include "mjpegDecoder.h"
#include "core/utils/preciseTimer.h"
#include "mjpegDecoderLazy.h"


const char* V4L2CaptureInterface::CODEC_NAMES[] =
{
    "off",
    "mjpeg",
    "mjpeg fast decoder"
};

STATIC_ASSERT(CORE_COUNT_OF(V4L2CaptureInterface::CODEC_NAMES) == V4L2CaptureInterface::CODEC_NUMBER, wrong_codec_names_number)

V4L2CaptureInterface::V4L2CaptureInterface(string _devname, bool isRgb)
    : spin(this)
{
    mIsRgb = isRgb;
    setConfigurationString(_devname);
}

V4L2CaptureInterface::V4L2CaptureInterface(string _devname, int h, int w, int fps, bool isRgb)
    : spin(this)
{
    mIsRgb = isRgb;
    interfaceName = QString("%1:1/%2:yuyv:%3x%4").arg(_devname.c_str()).arg(fps).arg(w).arg(h).toStdString();
    deviceName[Frames::LEFT_FRAME] =  _devname;

    decoder = UNCOMPRESSED;

    cameraMode.fpsnum     = 1;
    cameraMode.fpsdenum   = fps;
    cameraMode.width      = w;
    cameraMode.height     = h;
    cameraMode.compressed = false;
}

V4L2CaptureInterface::V4L2CaptureInterface(string _devname, ImageCaptureInterface::CameraFormat format, bool isRgb)
    : spin(this)
{
    mIsRgb = isRgb;
    interfaceName = QString("%1:1/%2:yuyv:%3x%4").arg(_devname.c_str()).arg(format.fps).arg(format.width).arg(format.height).toStdString();
    deviceName[Frames::LEFT_FRAME] =  _devname;

    decoder = UNCOMPRESSED;

    cameraMode.fpsnum     = 1;
    cameraMode.fpsdenum   = format.fps;
    cameraMode.width      = format.width;
    cameraMode.height     = format.height;
    cameraMode.compressed = false;
}

/**
 * Sets
 *   interfaceName
 *   deviceName
 *
 **/
int V4L2CaptureInterface::setConfigurationString(string _devname)
{
    this->interfaceName = _devname;

    //     Group Number                   1       2 3        4 5      6       7 8       9       10     11   12     13
    QRegExp deviceStringPattern(QString("^([^,:]*)(,([^:]*))?(:(\\d*)/(\\d*))?((:mjpeg)|(:yuyv)|(:fjpeg))?(:(\\d*)x(\\d*))?$"));
    static const int Device1Group     = 1;
    static const int Device2Group     = 3;
    static const int FpsNumGroup      = 5;
    static const int FpsDenumGroup    = 6;
    static const int CompressionGroup = 7;
    static const int WidthGroup       = 12;
    static const int HeightGroup      = 13;

    printf ("Input string %s\n", _devname.c_str());
    QString qdevname(_devname.c_str());
    int result = deviceStringPattern.indexIn(qdevname);
    if (result == -1)
    {
        printf("Error in device string format:%s\n", _devname.c_str());
        exit(EXIT_FAILURE);
    }
    printf (
        "Parsed data:\n"
        "  | - Device 1=%s\n"
        "  | - Device 2=%s\n"
        "  | - FPS %s/%s\n"
        "  | - Size [%sx%s]\n"
        "  \\ - Compressing: %s\n"
        "RGB decoding is %s\n",
        deviceStringPattern.cap(Device1Group) .toLatin1().constData(),
        deviceStringPattern.cap(Device2Group) .toLatin1().constData(),
        deviceStringPattern.cap(FpsNumGroup)  .toLatin1().constData(),
        deviceStringPattern.cap(FpsDenumGroup).toLatin1().constData(),
        deviceStringPattern.cap(WidthGroup)   .toLatin1().constData(),
        deviceStringPattern.cap(HeightGroup)  .toLatin1().constData(),
        deviceStringPattern.cap(CompressionGroup).toLatin1().constData(),
        mIsRgb ? "on" : "off"
    );

    deviceName[Frames::RIGHT_FRAME] = deviceStringPattern.cap(Device1Group).toLatin1().constData();
    deviceName[Frames::LEFT_FRAME ] = deviceStringPattern.cap(Device2Group).toLatin1().constData();

    bool isOk;
    cameraMode.fpsnum = deviceStringPattern.cap(FpsNumGroup).toInt(&isOk);
    if (!isOk) cameraMode.fpsnum = 1;

    cameraMode.fpsdenum = deviceStringPattern.cap(FpsDenumGroup).toInt(&isOk);
    if (!isOk) cameraMode.fpsdenum = 10;

    cameraMode.width = deviceStringPattern.cap(WidthGroup).toInt(&isOk);
    if (!isOk || cameraMode.width <= 0) cameraMode.width = 800;

    cameraMode.height = deviceStringPattern.cap(HeightGroup).toInt(&isOk);
    if (!isOk || cameraMode.height <= 0) cameraMode.height = 600;

    decoder = UNCOMPRESSED;
    if (!deviceStringPattern.cap(CompressionGroup).isEmpty())
    {
        if (!deviceStringPattern.cap(CompressionGroup).compare(QString(":mjpeg")))
            decoder = COMPRESSED_JPEG;
        if (!deviceStringPattern.cap(CompressionGroup).compare(QString(":fjpeg")))
            decoder = COMPRESSED_FAST_JPEG;
    }

    cameraMode.compressed = (decoder != UNCOMPRESSED);

    printf("Capture Right device: V4L2 %s\n", deviceName[Frames::RIGHT_FRAME].c_str());
    printf("Capture Left  device: V4L2 %s\n", deviceName[Frames::LEFT_FRAME ].c_str());
    printf("MJPEG compression is: %s\n", CODEC_NAMES[cameraMode.compressed]);
    return isOk ? 0 : 1;
}



V4L2CaptureInterface::FramePair V4L2CaptureInterface::getFrame()
{

//    SYNC_PRINT(("V4L2CaptureInterface::getFrame(): called\n"));
    CaptureStatistics  stats;

    PreciseTimer start = PreciseTimer::currentTime();
    FramePair result;

    protectFrame.lock();
    G12Buffer **results[Frames::MAX_INPUTS_NUMBER] = {
            &result.buffers[Frames::LEFT_FRAME ].g12Buffer,
            &result.buffers[Frames::RIGHT_FRAME].g12Buffer
    };

    result.setRgbBufferRight(NULL);
    result.setRgbBufferLeft (NULL);

    //SYNC_PRINT(("LF:%s RF:%s\n",
    //           currentFrame[Frames::LEFT_FRAME ].isFilled ? "filled" : "empty" ,
    //           currentFrame[Frames::RIGHT_FRAME].isFilled ? "filled" : "empty"));

    for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++)
    {
        decodeData(&camera[i],  &currentFrame[i],  results[i]);

        if ((*results[i]) == NULL) {
            SYNC_PRINT(("V4L2CaptureInterface::getFrame(): Precrash condition\n"));
        }
    }

    if (currentFrame[Frames::LEFT_FRAME].isFilled)
        result.setTimeStampLeft (currentFrame[Frames::LEFT_FRAME].usecsTimeStamp());

    if (currentFrame[Frames::RIGHT_FRAME].isFilled)
        result.setTimeStampRight   (currentFrame[Frames::RIGHT_FRAME].usecsTimeStamp());

    if (skippedCount == 0)
    {
        //SYNC_PRINT(("Warning: Requested same frames twice. Is this by design?\n"));
    }

    stats.framesSkipped = skippedCount > 0 ? skippedCount - 1 : 0;
    skippedCount = 0;
    protectFrame.unlock();

    stats.values[CaptureStatistics::DECODING_TIME] = start.usecsToNow();
    stats.values[CaptureStatistics::INTERFRAME_DELAY] = frameDelay;

    int64_t desync =  currentFrame[Frames::LEFT_FRAME].usecsTimeStamp() - currentFrame[Frames::RIGHT_FRAME].usecsTimeStamp();
    stats.values[CaptureStatistics::DESYNC_TIME] = desync > 0 ? desync : -desync;
    stats.values[CaptureStatistics::DATA_SIZE] = currentFrame[Frames::LEFT_FRAME].bytesused;

    if (imageInterfaceReceiver != NULL) {
        imageInterfaceReceiver->newStatisticsReadyCallback(stats);
    }

    return result;
}

V4L2CaptureInterface::FramePair V4L2CaptureInterface::getFrameRGB24()
{
    CaptureStatistics  stats;

    PreciseTimer start = PreciseTimer::currentTime();

    protectFrame.lock();

    FramePair result;

    RGB24Buffer **results[Frames::MAX_INPUTS_NUMBER] = {
        &result.buffers[LEFT_FRAME ].rgbBuffer,
        &result.buffers[RIGHT_FRAME].rgbBuffer
    };

    for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++)
    {
        decodeDataRGB24(&camera[i],  &currentFrame[i],  results[i]);

        if ((*results[i]) == NULL) {
            printf("V4L2CaptureInterface::getFrameRGB24(): Precrash condition at %d (%s)\n", i, Frames::getEnumName((Frames::FrameSourceId)i));
        }
    }

    if (result.rgbBufferLeft() != NULL) {
        result.setBufferLeft  ( result.rgbBufferLeft() ->toG12Buffer() ); // FIXME
    }
    if (result.rgbBufferRight() != NULL) {
        result.setBufferRight ( result.rgbBufferRight()->toG12Buffer() );
    }

    if (currentFrame[Frames::LEFT_FRAME].isFilled)
        result.setTimeStampLeft  (currentFrame[Frames::LEFT_FRAME].usecsTimeStamp());

    if (currentFrame[Frames::RIGHT_FRAME].isFilled)
        result.setTimeStampRight (currentFrame[Frames::RIGHT_FRAME].usecsTimeStamp());

    if (skippedCount == 0)
    {
   //     SYNC_PRINT(("Warning: Requested same frames twice. Is this by design?\n"));
    }

    stats.framesSkipped = skippedCount > 0 ? skippedCount - 1 : 0;
    skippedCount = 0;
    protectFrame.unlock();
    stats.values[CaptureStatistics::DECODING_TIME]    = start.usecsToNow();
    stats.values[CaptureStatistics::INTERFRAME_DELAY] = frameDelay;

    int64_t desync =  currentFrame[Frames::LEFT_FRAME ].usecsTimeStamp() -
                      currentFrame[Frames::RIGHT_FRAME].usecsTimeStamp();

    stats.values[CaptureStatistics::DESYNC_TIME] = CORE_ABS(desync);
    stats.values[CaptureStatistics::DATA_SIZE]   = currentFrame[Frames::LEFT_FRAME].bytesused;
    if (imageInterfaceReceiver != NULL) {
        imageInterfaceReceiver->newStatisticsReadyCallback(stats);
    } else {
        SYNC_PRINT(("Warning:  V4L2CaptureInterface::getFrameRGB24(): imageInterfaceReceiver is NULL\n"));
    }

    return result;
}


void V4L2CaptureInterface::SpinThread::run()
{
    while (mInterface->spinRunning.tryLock())
    {
        V4L2BufferDescriptor newBufferLeft;
        V4L2BufferDescriptor newBufferRight;

        V4L2CameraDescriptor* left  = &(mInterface->camera[Frames::LEFT_FRAME ]);
        V4L2CameraDescriptor* right = &(mInterface->camera[Frames::RIGHT_FRAME]);

        V4L2BufferDescriptor* currentLeft  = &(mInterface->currentFrame[Frames::LEFT_FRAME ]);
        V4L2BufferDescriptor* currentRight = &(mInterface->currentFrame[Frames::RIGHT_FRAME]);

        /* First we block till we will get a new frame */
        /*TODO: Wonder if dequeue should be static */
        left->dequeue(newBufferLeft);

        /* If we have only one camera, we assume this is the left camera */
        if (right->deviceHandle != V4L2CameraDescriptor::INVALID_HANDLE) {
            right->dequeue(newBufferRight);
        } else {
           // SYNC_PRINT(("V4L2CaptureInterface::SpinThread::run(): No right cam, not waiting or the new frame\n"));
        }

        uint64_t leftStamp = newBufferLeft.usecsTimeStamp();

        if (right->deviceHandle != V4L2CameraDescriptor::INVALID_HANDLE)
        {
            uint64_t rightStamp = newBufferRight.usecsTimeStamp();

            /*Reducing desync only  one frame at a time*/
            if (leftStamp > rightStamp && leftStamp - rightStamp > mInterface->maxDesync)
            {
                right->enqueue(newBufferRight);
                right->dequeue(newBufferRight);
                rightStamp = newBufferRight.usecsTimeStamp();
            }

            if (rightStamp > leftStamp && rightStamp - leftStamp > mInterface->maxDesync)
            {
                left->enqueue(newBufferLeft);
                left->dequeue(newBufferLeft);
                leftStamp = newBufferLeft.usecsTimeStamp();
            }
        }

        /**
         *  Now we have two new buffers
         *  So we do some thread protected operations.
         *  1. Enqueue the previously used buffers
         *  2. Replace them with the new ones
         **/
        mInterface->protectFrame.lock();
        {
            if (left->deviceHandle != -1)
            {
                left->enqueue(*currentLeft);
                *currentLeft = newBufferLeft;
            }

            if (right->deviceHandle != -1)
            {
                right->enqueue(*currentRight);
                *currentRight = newBufferRight;
            }
            mInterface->skippedCount++;
        }
        mInterface->protectFrame.unlock();


        /* For statistics */
        if (mInterface->lastFrameTime.usecsTo(PreciseTimer()) != 0)
        {
            mInterface->frameDelay = mInterface->lastFrameTime.usecsToNow();
        }
        mInterface->lastFrameTime = PreciseTimer::currentTime();

//        cout << "Frame notification" << endl;
        frame_data_t frameData;
        frameData.timestamp = (currentLeft->usecsTimeStamp() / 2) + (currentRight->usecsTimeStamp() / 2);

        mInterface->notifyAboutNewFrame(frameData);

        mInterface->spinRunning.unlock();
        if (mInterface->shouldStopSpinThread)
        {
            break;
        }
    }
}

uint16_t *V4L2CaptureInterface::decodeMjpeg(unsigned char *data)
{
    int width  = 800;
    int height = 600;
    unsigned char * framebuffer = 0;

    MjpegDecoder decoder;
    int result = decoder.decode(&framebuffer, data, &width, &height);
    if (result != 0)
    {
        SYNC_PRINT(("MJPEG decoder error, code %d\n", result));
    }

    return (uint16_t*)framebuffer;
}

void V4L2CaptureInterface::decodeData(V4L2CameraDescriptor *camera, V4L2BufferDescriptor *buffer, G12Buffer **output)
{
    if (!buffer->isFilled)
    {
    //    SYNC_PRINT(("V4L2CaptureInterface::decodeData(): Buffer is not filled. Returning empty\n"));
        *output = new G12Buffer(formatH, formatW, false);
        return;
    }

    //SYNC_PRINT(("V4L2CaptureInterface::decodeData(): Decoding buffer\n"));

    uint8_t *ptrL = (uint8_t*)(camera->buffers[buffer->index].start);
    switch(decoder)
    {
        case UNCOMPRESSED:
            *output = new G12Buffer(formatH, formatW, false);
            (*output)->fillWithYUYV((uint16_t *)ptrL);
            break;
        case COMPRESSED_JPEG:
        {
            uint16_t *ptrDecoded = decodeMjpeg(ptrL);
            *output = new G12Buffer(formatH, formatW, false);
            (*output)->fillWithYUYV(ptrDecoded);
            free(ptrDecoded);
        }
        break;
        case CODEC_NUMBER:
        case COMPRESSED_FAST_JPEG:
        {
            MjpegDecoderLazy lazyDecoder;
            *output = lazyDecoder.decode(ptrL);
            if (*output == NULL) {
//                SYNC_PRINT(("V4L2CaptureInterface::decodeData(): Decoded to buffer that is NULL\n"));
            }
        }
        break;
    }
}

void V4L2CaptureInterface::decodeDataRGB24(V4L2CameraDescriptor *camera, V4L2BufferDescriptor *buffer, RGB24Buffer **output)
{
    if (!buffer->isFilled)
    {
        *output = new RGB24Buffer(formatH, formatW);
        return;
    }

    uint8_t *ptrL = (uint8_t*)(camera->buffers[buffer->index].start);
    PreciseTimer timer;
    switch(decoder)
    {
        case UNCOMPRESSED:
            *output = new RGB24Buffer(formatH, formatW);
//            printf("Decoding image...");
            timer = PreciseTimer::currentTime();
#if 0
            for (int i = 0; i < formatH; i++)
            {
                uint8_t *ptrI = ptrL + formatW * 2 * i;
                RGBColor *ptrO = &((*output)->element(i,0));

                for (int j = 0; j < formatW; j += 2)
                {
                    int y1 = ptrI[0];
                    int u  = ptrI[1];
                    int y2 = ptrI[2];
                    int v  = ptrI[3];
                    ptrI += 4;
                    ptrO[j    ] = RGBColor::FromYUV(y1, u, v);
                    ptrO[j + 1] = RGBColor::FromYUV(y2, u, v);
                }
            }
#endif
            (*output)->fillWithYUYV(ptrL);
//            printf("Delay: %i\n", timer.usecsToNow());
            break;
        case COMPRESSED_JPEG:
        {
            /*uint16_t *ptrDecoded = decodeMjpeg(ptrL );
            *output = new G12Buffer(formatH, formatW, false);
            (*output)->fillWithYUYV(ptrDecoded);
            free(ptrDecoded);*/
            SYNC_PRINT(("V4L2CaptureInterface::decodeDataRGB24(): COMPRESSED_JPEG not supported"));

        }
        break;
        case CODEC_NUMBER:
            SYNC_PRINT(("V4L2CaptureInterface::decodeDataRGB24(): CODEC_NUMBER not supported"));
            break;
        case COMPRESSED_FAST_JPEG:
            SYNC_PRINT(("V4L2CaptureInterface::decodeDataRGB24(): COMPRESSED_JPEG not supported"));
            break;
        default:
            SYNC_PRINT(("V4L2CaptureInterface::decodeDataRGB24(): %d decoder not supported", decoder));
            break;

    }
}

ImageCaptureInterface::CapErrorCode V4L2CaptureInterface::queryCameraParameters(CameraParameters &parameter)
{
    camera[Frames::LEFT_FRAME].queryCameraParameters(parameter);
    return SUCCESS;
}


ImageCaptureInterface::CapErrorCode V4L2CaptureInterface::setCaptureProperty(int id, int value)
{
    for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++) {
        camera[i].setCaptureProperty(id, value);
    }
    return SUCCESS;
}

ImageCaptureInterface::CapErrorCode V4L2CaptureInterface::getCaptureProperty(int id, int *value)
{
    camera[Frames::LEFT_FRAME].getCaptureProperty(id, value);
    //printf("Getting the values of capture properties is not supported for V4L2 yet.\n");
    return SUCCESS;
}

ImageCaptureInterface::CapErrorCode V4L2CaptureInterface::initCapture()
{
    formatH = 0;
    formatW = 0;

    /* TODO: Check that formats are ok*/

    maxDesync = (1000000UL * cameraMode.fpsnum / cameraMode.fpsdenum) * 6 / 10;
    printf("Max allowed desync is %" PRIu64 "\n", maxDesync );

    skippedCount = 0;
    frameDelay = 0;
    shouldStopSpinThread = false;

    bool initOk[Frames::MAX_INPUTS_NUMBER];

    for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++) {
        initOk[i] = false;
        do {
            if (deviceName[i].empty()) {
                SYNC_PRINT(("Device %d name is empty\n", i));
                break;
            }
            if (camera[i].initCamera(deviceName[i], cameraMode))
            {
                SYNC_PRINT(("Initing device %s failed\n", deviceName[i].c_str()));
                break;
            }
            if (camera[i].initBuffers()) {
                SYNC_PRINT(("Initing buffers for device %s failed\n", deviceName[i].c_str()));
                break;
            }
            SYNC_PRINT(("Device %d init sequence done\n", i));
            initOk[i] = true;
        } while (0);


        if (formatH == 0) {
            formatH = camera[i].formatH;
            formatW = camera[i].formatW;
        }
        else if (formatH != camera[i].formatH || formatW != camera[i].formatW)
        {
            SYNC_PRINT(( "Cameras initialized to different sizes [%dx%d] and [%dx%d]\n", formatW, formatH, camera[i].formatH, camera[i].formatW ));
        }
    }

    /* If only one camera started, we assume it is the left camera */
    if ((!initOk[Frames::LEFT_FRAME]) && (initOk[Frames::RIGHT_FRAME]))
    {
        SYNC_PRINT(("Inited one camera, making it the first one\n"));
        V4L2CameraDescriptor tmp;
        tmp = camera[Frames::LEFT_FRAME];
        camera[Frames::LEFT_FRAME]  = camera[Frames::RIGHT_FRAME];
        camera[Frames::RIGHT_FRAME] = tmp;
    }

    SYNC_PRINT(("Resumee:\n"));
    for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++) {
        SYNC_PRINT(("  Device %d (%s) init %s\n", i, deviceName[i].c_str(), initOk[i] ? "Ok" : "Fail"));
    }

    if (initOk[Frames::LEFT_FRAME] && initOk[Frames::RIGHT_FRAME])
        return SUCCESS;
    if (initOk[Frames::LEFT_FRAME] || initOk[Frames::RIGHT_FRAME])
        return SUCCESS_1CAM;
    return FAILURE;
}

ImageCaptureInterface::CapErrorCode V4L2CaptureInterface::startCapture()
{
    int res[Frames::MAX_INPUTS_NUMBER];

    /*Try to start as synchronous as possible*/
    for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++) {
        res[i] = camera[i].start();
    }

    shouldStopSpinThread = false;
    spin.start();

    return (CapErrorCode) ((bool) res[Frames::LEFT_FRAME] + (bool) res[Frames::RIGHT_FRAME]);
}

ImageCaptureInterface::CapErrorCode V4L2CaptureInterface::getCaptureName(string &value)
{
    string name;
    if (camera[0].getCaptureName(name) != 0)
    {
        camera[1].getCaptureName(name);
    }
    value = name;
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode V4L2CaptureInterface::getFormats(int *num, ImageCaptureInterface::CameraFormat *&formats)
{
    if (camera[0].getCaptureFormats(num, formats) != 0)
    {
        camera[1].getCaptureFormats(num, formats);
    }
    return ImageCaptureInterface::SUCCESS;
}

string V4L2CaptureInterface::getInterfaceName()
{
    return std::string("v4l2:") + interfaceName;
}

ImageCaptureInterface::CapErrorCode V4L2CaptureInterface::getDeviceName(int num, string &name)
{
    if (num  < 0 || num >= Frames::MAX_INPUTS_NUMBER)
    {
        return ImageCaptureInterface::FAILURE;
    }
    name = deviceName[num];
    return ImageCaptureInterface::SUCCESS;
}

string V4L2CaptureInterface::getDeviceSerial(int num)
{
    if (num  < 0 || num >= Frames::MAX_INPUTS_NUMBER)
    {
        return "wrong device";
    }
    if (camera[num].deviceHandle == V4L2CameraDescriptor::INVALID_HANDLE)
    {
        return "null";
    }
    return camera[num].getSerialNumber();
}

void V4L2CaptureInterface::getAllCameras(int *num, int *&cameras)
{
    vector<int> allCameras;
    const int maxUsbPortNum = 2;
    for (int i = 0; i < maxUsbPortNum; i ++)
    {
        std::stringstream ss;
        ss << i;
        string dev = "/dev/video" + ss.str();
        V4L2CameraDescriptor cameraDescriptor;
        if (cameraDescriptor.initCamera(dev, 480, 640, 1, 30, false) == 0 ||
            cameraDescriptor.initCamera(dev, 480, 640, 1, 30, true) == 0)
        {
            allCameras.push_back(i);
        }
    }
    *num = allCameras.size();
    cameras = new int[allCameras.size()];
    for (unsigned i = 0; i < allCameras.size(); i ++)
    {
        cameras[i] = allCameras[i];
    }
}



void V4L2CaptureInterface::getAllCameras(vector<std::string> &cameras, int maxDeviceId)
{

    for (int i = 0; i < maxDeviceId; i ++)
    {
        /* Stupid C++99 */
        std::stringstream ss;
        ss << i;
        string dev = "/dev/video" + ss.str();

        /* It is discussable if we should check initailization process */
        bool isActive = false;
        {
            V4L2CameraDescriptor cameraDescriptor;
            if (cameraDescriptor.initCamera(dev, 480, 640, 1, 30, false) == 0) {
                isActive = true;
            }
        }
        {
            V4L2CameraDescriptor cameraDescriptor;
            if (cameraDescriptor.initCamera(dev, 480, 640, 1, 30, true) == 0) {
                isActive = true;
                /*std::string serial = cameraDescriptor.getSerialNumber();
                printf("Serial is <%s>", serial.c_str());*/
            }
        }

        if (isActive) {
            cameras.push_back(dev);
        }
    }
}



void V4L2CaptureInterface::setCaptureDeviceParameters(const int handle, const int prop,
                                                      const int32_t val, int &res, const char* text) const
{
    v4l2_control request;
    if (handle)
    {
        request.id = prop;
        request.value = val;
        int result = ioctl( handle, VIDIOC_S_CTRL, &request);
        if ( result != 0 )
        {
            SYNC_PRINT(( "VIDIOC_S_CTRL for %s with value %d failed. (%s)\n", text, val, strerror(errno) ));
        }

        res |= result;
    }
    else {
        res = -1;
    }
}


V4L2CaptureInterface::~V4L2CaptureInterface()
{
   // spinRunning.lock();
    cout << "V4L2CaptureInterface::Request for killing the thread" << endl;

    shouldStopSpinThread = true;
    bool result = spinRunning.tryLock(1000);

    if (result) {
        SYNC_PRINT(("V4L2CaptureInterface::Camera thread killed\n"));
    } else {
        SYNC_PRINT(("V4L2CaptureInterface::Unable to exit Camera thread\n"));
    }

    for (int i = 0; i < Frames::MAX_INPUTS_NUMBER; i++) {
        SYNC_PRINT(("V4L2CaptureInterface::Stopping cameras\n"));
        camera[i].stop();
    }
}
