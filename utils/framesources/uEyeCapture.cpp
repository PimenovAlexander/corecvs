/**
 * \file UEyeCapture.cpp
 * \brief Add Comment Here
 *
 * \date March 15, 2012
 * \author kat
 */

#define __STDC_FORMAT_MACROS
//#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

#include <fcntl.h>
//#include <unistd.h>
#include <stdlib.h>
//#include <sys/mman.h>
//#include <sys/ioctl.h>
//#include <linux/videodev2.h>
#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QtCore>

#include "core/utils/global.h"

#include "uEyeCapture.h"
#include "core/utils/preciseTimer.h"


#ifdef PROFILE_DEQUEUE
#define TRACE_DEQUEUE(X) printf X
#else
#define TRACE_DEQUEUE(X)
#endif


#ifdef DEEP_TRACE_ON
#define DEEP_TRACE(X) SYNC_PRINT (X)
#else
#define DEEP_TRACE(X)
#endif

/*static*/ const double UEyeCaptureInterface::EXPOSURE_SCALER = 10.0;
/*static*/ const double UEyeCaptureInterface::FPS_SCALER      = 100.0;

int UEyeCaptureInterface::ueyeTrace(int result, const char *prefix)
{
    if (result != IS_SUCCESS)
    {
        char * str = 0;
        is_GetError(1,&result,&str);
        if (prefix == NULL)
            printf("   UEYE: Function failed with error code %d. Error string: %s\n", result, str);
        else
            printf("%s: error code %d. Error string: %s\n", prefix, result, str);

    }

    return result;
}




UEyeCaptureInterface::UEyeCaptureInterface(string _devname) :
    sync(NO_SYNC),
    currentLeft(NULL),
    currentRight(NULL),
    spin(this)
{
    setConfigurationString(_devname);
}

UEyeCaptureInterface::UEyeCaptureInterface(string _devname, int /*h*/, int /*w*/, int /*fps*/, bool isRgb) :
    sync(NO_SYNC),
    currentLeft(NULL),
    currentRight(NULL),
    spin(this)
{
    setConfigurationString(_devname, isRgb);
}

int UEyeCaptureInterface::setConfigurationString(string _devname, bool isRgb)
{
    interfaceName = _devname;
    printf("#################################################   \n");
    printf("Hi, Stranger. I will help you with the uEye cameras.\n");
    printf("Remember, everything is in the eye of the beholder. \n");
    printf("#################################################   \n");
    int camNum = 0;
    is_GetNumberOfCameras (&camNum);
    printf("Currently there are %d cameras in the system:\n", camNum);
    UEYE_CAMERA_LIST *camList = (UEYE_CAMERA_LIST *)malloc(sizeof(ULONG) + camNum * sizeof(UEYE_CAMERA_INFO));
    camList->dwCount = camNum;
    is_GetCameraList(camList);

    printf("%2s %16s %5s %5s %5s %6s %16s\n",
            "N", "Model" ,"camId", "devId" ,"sensId", "inuse?", "Serial");

    for (int i = 0; i < camNum; i++)
    {
        printf("% 2d %16s % 5u % 5u %5u %6s %16s\n",
            i,
            camList->uci[i].Model,
            camList->uci[i].dwCameraID,
            camList->uci[i].dwDeviceID,
            camList->uci[i].dwSensorID,
            camList->uci[i].dwInUse != 0 ? "used" : "free",
            camList->uci[i].SerNo);

    }
    free(camList);


    this->devname = _devname;

    //     Group Number                   1       2 3        4 5          6 7             8 910   11     12 13     14 15                 16 17
    QRegExp deviceStringPattern(QString("^([^,:]*)(,([^:]*))?(:(\\d*)mhz)?(:([0-9.]*)fps)?(:((10)|(8))b)?(:x(\\d*))?(:(sync|hsync|fsync|esync))?(:(global))?$"));
    static const int Device1Group       = 1;
    static const int Device2Group       = 3;
    static const int PixelClockGroup    = 5;
    static const int FpsGroup           = 7;
    static const int BitsGroup          = 9;
    static const int BinningGroup       = 13;
    static const int SyncGroup          = 15;
    static const int GlobalShutterGroup = 17;

    //static const int WidthGroup       = 12;
    //static const int HeightGroup      = 13;

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
        "  | - Pixelclock %s Mhz, FPS %s\n"
        "  | - BPP:%s with bining %s\n"
        "  | - Sync:%s\n"
        "  \\ - GlobalShutter:%s\n",

        deviceStringPattern.cap(Device1Group).toLatin1().constData(),
        deviceStringPattern.cap(Device2Group).toLatin1().constData(),
        deviceStringPattern.cap(PixelClockGroup).toLatin1().constData(),
        deviceStringPattern.cap(FpsGroup).toLatin1().constData(),
        deviceStringPattern.cap(BitsGroup).toLatin1().constData(),
        deviceStringPattern.cap(BinningGroup).toLatin1().constData(),
        deviceStringPattern.cap(SyncGroup).toLatin1().constData(),
        deviceStringPattern.cap(GlobalShutterGroup).toLatin1().constData()
    );

    string leftName  = deviceStringPattern.cap(Device1Group).toLatin1().constData();
    string rightName = deviceStringPattern.cap(Device2Group).toLatin1().constData();
    bool err;

    int leftID =  deviceStringPattern.cap(Device1Group).toInt(&err);
    if (!err) {
        printf("Wrong left device ID (%s)\n", leftName.c_str());
    }

    int rightID =  deviceStringPattern.cap(Device2Group).toInt(&err);
    if (!err) {
        printf("Wrong right device ID (%s)\n", rightName.c_str());
    }

    int pixelClock = deviceStringPattern.cap(PixelClockGroup).toInt(&err);
    if (!err) pixelClock = 85;

    double fps = deviceStringPattern.cap(FpsGroup).toDouble(&err);
    if (!err) fps = 50.0;


/*    int width = deviceStringPattern.cap(WidthGroup).toInt(&err);
    if (!err || width <= 0) width = 800;

    int height = deviceStringPattern.cap(HeightGroup).toInt(&err);
    if (!err || height <= 0) height = 600;*/


    int binning = 1;
    binning = deviceStringPattern.cap(BinningGroup).toInt(&err);
    if (!err) binning = 1;

    sync = NO_SYNC;

    if (!deviceStringPattern.cap(SyncGroup).compare(QString("sync")))
        sync = SOFT_SYNC;

    if (!deviceStringPattern.cap(SyncGroup).compare(QString("hsync")))
        sync = HARD_SYNC;

    if (!deviceStringPattern.cap(SyncGroup).compare(QString("fsync")))
        sync = FRAME_HARD_SYNC;

    if (!deviceStringPattern.cap(SyncGroup).compare(QString("esync")))
        sync = EXT_SYNC;


    bool  globalShutter = false;
    if (!deviceStringPattern.cap(GlobalShutterGroup).compare(QString("global")))
        globalShutter = true;

    printf("Shutter is:%s\n", globalShutter ? "global" : "rolling");

    printf("Capture Left  device: GigE DeviceID = %d\n", leftID);
    printf("Capture Right device: GigE DeviceID = %d\n", rightID);



    leftCamera .init( leftID, binning, globalShutter, pixelClock, fps, isRgb);
    rightCamera.init(rightID, binning, globalShutter, pixelClock, fps, isRgb);

    skippedCount = 0;
    triggerSkippedCount = 0;
    frameDelay = 0;

    return 0;
}

UEyeCaptureInterface::FramePair UEyeCaptureInterface::getFrame()
{
    CaptureStatistics  stats;
    PreciseTimer start = PreciseTimer::currentTime();
    FramePair result( NULL, NULL);

//    printf("Called getFrame\n");

    protectFrame.lock();
        decodeData(&leftCamera , currentLeft,  &(result.bufferLeft));
        decodeData(&rightCamera, currentRight, &(result.bufferRight));

        int64_t internalDesync = 0;

        if (rightCamera.inited) {
            result.timeStampLeft  = currentRight->usecsTimeStamp();
            result.timeStampRight = currentRight->usecsTimeStamp();
            internalDesync = currentLeft->internalTimestamp - currentRight->internalTimestamp;
        } else {
            result.timeStampLeft  = currentLeft->usecsTimeStamp();
            result.timeStampRight = currentLeft->usecsTimeStamp();
        }

        stats.framesSkipped = skippedCount > 0 ? skippedCount - 1 : 0;
        skippedCount = 0;

        stats.triggerSkipped = triggerSkippedCount;
        triggerSkippedCount = 0;
    protectFrame.unlock();

    stats.values[CaptureStatistics::DECODING_TIME]    = start.usecsToNow();
    stats.values[CaptureStatistics::INTERFRAME_DELAY] = frameDelay;

    int64_t desync = result.diffTimeStamps();
    stats.values[CaptureStatistics::DESYNC_TIME]          = CORE_ABS(desync);
    stats.values[CaptureStatistics::INTERNAL_DESYNC_TIME] = CORE_ABS(internalDesync);

    /* Get temperature data */
    stats.temperature[0] = leftCamera .getTemperature();    
    stats.temperature[1] = stats.temperature[0];
    if (rightCamera.inited) {
        stats.temperature[1] = rightCamera.getTemperature();
    }


    //stats.values[CaptureStatistics::DATA_SIZE] = currentLeft.bytesused;

    if (imageInterfaceReceiver != NULL) {
        imageInterfaceReceiver->newStatisticsReadyCallback(stats);
    }

//    printf("Finished getFrame\n");
    return result;
}


UEyeCaptureInterface::FramePair UEyeCaptureInterface::getFrameRGB24()
{
    CaptureStatistics  stats;
    PreciseTimer start = PreciseTimer::currentTime();
    FramePair result( NULL, NULL);

//    printf("Called getFrame\n");

    protectFrame.lock();
        decodeData24(&leftCamera , currentLeft,  &(result.rgbBufferLeft));
        decodeData24(&rightCamera, currentRight, &(result.rgbBufferRight));

        if (result.rgbBufferLeft != NULL) {
            result.bufferLeft  = result.rgbBufferLeft ->toG12Buffer(); // FIXME
        }
        if (result.rgbBufferRight != NULL) {
            result.bufferRight = result.rgbBufferRight->toG12Buffer();
        }

        int64_t internalDesync = 0;

        if (rightCamera.inited) {
            result.timeStampLeft  = currentRight->usecsTimeStamp();
            result.timeStampRight = currentRight->usecsTimeStamp();
            internalDesync = currentLeft->internalTimestamp - currentRight->internalTimestamp;
        } else {
            result.timeStampLeft  = currentLeft->usecsTimeStamp();
            result.timeStampRight = currentLeft->usecsTimeStamp();
        }

        stats.framesSkipped = skippedCount > 0 ? skippedCount - 1 : 0;
        skippedCount = 0;

        stats.triggerSkipped = triggerSkippedCount;
        triggerSkippedCount = 0;
    protectFrame.unlock();

    stats.values[CaptureStatistics::DECODING_TIME]    = start.usecsToNow();
    stats.values[CaptureStatistics::INTERFRAME_DELAY] = frameDelay;

    int64_t desync = result.diffTimeStamps();
    stats.values[CaptureStatistics::DESYNC_TIME]          = CORE_ABS(desync);
    stats.values[CaptureStatistics::INTERNAL_DESYNC_TIME] = CORE_ABS(internalDesync);

    /* Get temperature data */
    stats.temperature[0] = leftCamera .getTemperature();
    stats.temperature[1] = stats.temperature[0];
    if (rightCamera.inited) {
        stats.temperature[1] = rightCamera.getTemperature();
    }


    //stats.values[CaptureStatistics::DATA_SIZE] = currentLeft.bytesused;

    if (imageInterfaceReceiver)
        imageInterfaceReceiver->newStatisticsReadyCallback(stats);

//    printf("Finished getFrame\n");
    return result;
}


UEyeCaptureInterface::~UEyeCaptureInterface()
{
    cout << "Request for killing the thread" << endl;
    shouldStopSpinThread = true;
    bool result = spinRunning.tryLock(2000);

    is_DisableEvent(leftCamera .mCamera, IS_SET_EVENT_FRAME);
    if (rightCamera.inited)
        is_DisableEvent(rightCamera.mCamera, IS_SET_EVENT_FRAME);

#ifdef Q_OS_WIN
    is_ExitEvent (leftCamera .mCamera, IS_SET_EVENT_FRAME);
    is_ExitEvent (rightCamera.mCamera, IS_SET_EVENT_FRAME);

    CloseHandle(leftCamera .mWinEvent);
    CloseHandle(rightCamera.mWinEvent);
#endif

    /* Stopping camera */
    if (sync == NO_SYNC)
    {
        ueyeTrace(is_StopLiveVideo(leftCamera.mCamera, IS_FORCE_VIDEO_STOP), "is_StopLiveVideo left camera");
        if (rightCamera.inited)
            ueyeTrace(is_StopLiveVideo(rightCamera.mCamera, IS_FORCE_VIDEO_STOP), "is_StopLiveVideo right camera");
    }

    /* Now no events would be generated, and it is safe to unlock mutex */

    if (result) {
        printf("Camera thread killed\n");
        spinRunning.unlock();
    } else {
        printf("Unable to exit Camera thread\n");
    }

    printf("Deleting image buffers\n");

    leftCamera.deallocImages();
    if (rightCamera.inited)
        rightCamera.deallocImages();

    printf("uEye: Closing camera...");
    ueyeTrace(is_ExitCamera(leftCamera.mCamera), "is_ExitCamera left camera");
    if (rightCamera.inited)
        ueyeTrace(is_ExitCamera(rightCamera.mCamera), "is_ExitCamera right camera");
    printf("done\n");

}

ImageCaptureInterface::CapErrorCode UEyeCaptureInterface::startCapture()
{
    SYNC_PRINT((" UEyeCaptureInterface::startCapture(): called\n"));

    SYNC_PRINT(("Enabling events...\n"));
#ifdef Q_OS_WIN
    leftCamera.mWinEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    is_InitEvent(leftCamera.mCamera, leftCamera.mWinEvent, IS_SET_EVENT_FRAME);

    rightCamera.mWinEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    is_InitEvent(rightCamera.mCamera, rightCamera.mWinEvent, IS_SET_EVENT_FRAME);
#endif

    SYNC_PRINT(("Enable events for left camera\n"));
    ueyeTrace(is_EnableEvent(leftCamera .mCamera, IS_SET_EVENT_FRAME), "is_EnableEvent left cam");
    if (rightCamera.inited) {
        SYNC_PRINT(("Enable events for right camera\n"));
        ueyeTrace(is_EnableEvent(rightCamera.mCamera, IS_SET_EVENT_FRAME), "is_EnableEvent right cam");
    }

    SYNC_PRINT(("Starting stero pair in sync mode (%d)...\n", sync));
    switch (sync)
    {
        case NO_SYNC:
            SYNC_PRINT(("Will start in non-sync mode\n"));
            SYNC_PRINT(("Start for left camera\n"));
            ueyeTrace(is_CaptureVideo (leftCamera .mCamera, IS_DONT_WAIT), "is_CaptureVideo left cam");
            if (rightCamera.inited) {
                SYNC_PRINT(("Start for right camera\n"));
                ueyeTrace(is_CaptureVideo (rightCamera.mCamera, IS_DONT_WAIT), "is_CaptureVideo right cam");
            }
            break;
        case SOFT_SYNC:
            SYNC_PRINT(("Will start in trigger mode\n"));
            SYNC_PRINT(("Will set trigger for left camera\n"));
            ueyeTrace(is_SetExternalTrigger (leftCamera.mCamera , IS_SET_TRIGGER_SOFTWARE));
            if (rightCamera.inited) {
                SYNC_PRINT(("Will set trigger for right camera\n"));
                ueyeTrace(is_SetExternalTrigger (rightCamera.mCamera, IS_SET_TRIGGER_SOFTWARE));
            }
            break;
        case HARD_SYNC:
        {
            /* Should make a check that connection exist */
            /*Left camera will generate flash*/
            UINT nMode = IO_FLASH_MODE_FREERUN_HI_ACTIVE;
            printf("Setting left flash IO_FLASH_MODE_FREERUN_HI_ACTIVE...\n");
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode)));

            /* Switching on leftCamera flash that will become right camera trigger */
            printf("Setting left flash duration and delay...\n");
            IO_FLASH_PARAMS flashParams;
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 5000;  // in us
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));


            /*right camera will generate flash also (for debug)*/
            nMode = IO_FLASH_MODE_FREERUN_HI_ACTIVE;
            printf("Setting right flash IO_FLASH_MODE_FREERUN_HI_ACTIVE...\n");
            ueyeTrace(is_IO(rightCamera.mCamera, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode)));

            /* Switching on leftCamera flash that will become right camera trigger */
            printf("Setting right flash duration and delay...\n");
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 5000;
            ueyeTrace(is_IO(rightCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));

            /* Right camera is trigger based */
            ueyeTrace(is_SetExternalTrigger (rightCamera.mCamera, IS_SET_TRIGGER_LO_HI));
            //ueyeTrace(is_SetExternalTrigger (rightCamera.mCamera, IS_SET_TRIGGER_HI_LO));

			printf("Launching both cameras run...\n");
			/* Left camera is in free run mode */
			ueyeTrace(is_CaptureVideo (leftCamera.mCamera, IS_DONT_WAIT));
			/* Same for the right camera */
			ueyeTrace(is_CaptureVideo (rightCamera.mCamera, IS_DONT_WAIT));
        	break;
        }
        case FRAME_HARD_SYNC:
        {
            /* TODO: Should make a check that connection exist */
        	/* Both cams are now trigger based*/
            ueyeTrace(is_SetExternalTrigger (leftCamera.mCamera, IS_SET_TRIGGER_OFF));

            /*Left camera will generate flash*/
            UINT nMode = IO_FLASH_MODE_FREERUN_HI_ACTIVE;
            printf("Setting left flash IO_FLASH_MODE_TRIGGER_HI_ACTIVE...\n");
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode)));

            /* Switching on leftCamera flash that will become right camera trigger */
            printf("Setting left flash duration and delay...\n");
            IO_FLASH_PARAMS flashParams;
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 5000;
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));

            printf("Setting right camera to TRIGGER MODE...\n");
            ueyeTrace(is_SetExternalTrigger (rightCamera.mCamera, IS_SET_TRIGGER_LO_HI   ));

            /*right camera will generate flash also (for debug)*/
            nMode = IO_FLASH_MODE_TRIGGER_HI_ACTIVE;
            printf("Setting right flash IO_FLASH_MODE_TRIGGER_HI_ACTIVE...\n");
            ueyeTrace(is_IO(rightCamera.mCamera, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode)));

            /* Switching on leftCamera flash that will become right camera trigger */
            printf("Setting right flash duration and delay...\n");
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 5000;
            ueyeTrace(is_IO(rightCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
            break;
        }
        case EXT_SYNC:
        {
            printf("Running on external syncro...\n");
        	/* Setting triggers for both cams*/
            printf("Setting left camera to TRIGGER MODE...\n");
            ueyeTrace(is_SetExternalTrigger (leftCamera.mCamera, IS_SET_TRIGGER_LO_HI));
            printf("Setting right camera to TRIGGER MODE...\n");
            ueyeTrace(is_SetExternalTrigger (rightCamera.mCamera,IS_SET_TRIGGER_LO_HI));

            /*Left camera will generate flash*/
            UINT nMode = IO_FLASH_MODE_TRIGGER_HI_ACTIVE;
            printf("Setting left flash IO_FLASH_MODE_TRIGGER_HI_ACTIVE...\n");
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode)));

            /* Switching on leftCamera flash that will become right camera trigger */
            printf("Setting left flash duration and delay...\n");
            IO_FLASH_PARAMS flashParams;
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 5000;
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));

            /*right camera will generate flash also (for debug)*/
            nMode = IO_FLASH_MODE_TRIGGER_HI_ACTIVE;
            printf("Setting right flash IO_FLASH_MODE_TRIGGER_HI_ACTIVE...\n");
            ueyeTrace(is_IO(rightCamera.mCamera, IS_IO_CMD_FLASH_SET_MODE, (void*)&nMode, sizeof(nMode)));

            /* Switching on leftCamera flash that will become right camera trigger */
            printf("Setting right flash duration and delay...\n");
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 5000;
            ueyeTrace(is_IO(rightCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));

            /* Left camera is repeated run mode */
			ueyeTrace(is_CaptureVideo (leftCamera.mCamera, IS_DONT_WAIT));
			/* Same for the right camera */
			ueyeTrace(is_CaptureVideo (rightCamera.mCamera, IS_DONT_WAIT));
            break;

        }

    }

    shouldStopSpinThread = false;
    spin.start();
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode UEyeCaptureInterface::getFormats(int *num, ImageCaptureInterface::CameraFormat *&formats)
{
    SYNC_PRINT(("UEyeCaptureInterface::getFormats()\n"));
    vector<ImageCaptureInterface::CameraFormat> cameraFormats;

    cameraFormats.push_back(ImageCaptureInterface::CameraFormat( 3684, 4912, 21));
    cameraFormats.push_back(ImageCaptureInterface::CameraFormat( 1842, 2448, 54));

    *num = cameraFormats.size();
    formats = new ImageCaptureInterface::CameraFormat[cameraFormats.size()];
    for (unsigned i = 0; i < cameraFormats.size(); i ++)
    {
        formats[i] = cameraFormats[i];
    }
    return ImageCaptureInterface::SUCCESS;
}

void UEyeCaptureInterface::getAllCameras(vector<string> &cameras)
{
    int camNum = 0;
    is_GetNumberOfCameras (&camNum);
    printf("Currently there are %d cameras in the system:\n", camNum);
    UEYE_CAMERA_LIST *camList = (UEYE_CAMERA_LIST *)malloc(sizeof(ULONG) + camNum * sizeof(UEYE_CAMERA_INFO));
    camList->dwCount = camNum;
    is_GetCameraList(camList);

    printf("%2s %16s %5s %5s %5s %6s %16s\n",
            "N", "Model" ,"camId", "devId" ,"sensId", "inuse?", "Serial");

    for (int i = 0; i < camNum; i++)
    {
        printf("% 2d %16s % 5d % 5d %5d %6s %16s\n",
            i,
            camList->uci[i].Model,
            camList->uci[i].dwCameraID,
            camList->uci[i].dwDeviceID,
            camList->uci[i].dwSensorID,
            camList->uci[i].dwInUse != 0 ? "used" : "free",
            camList->uci[i].SerNo);

        std::stringstream ss;
        ss << camList->uci[i].dwDeviceID << ",-1:400mhz:19fps";
        string dev = ss.str();
        cameras.push_back(dev);

    }
    free(camList);

}

QString UEyeCaptureInterface::getInterfaceName()
{
    return QString("ueye:") + QString(interfaceName.c_str());
}


string UEyeCaptureInterface::getDeviceSerial(int /*num*/)
{
    SYNC_PRINT(("UEyeCaptureInterface::getDeviceSerial():called\n"));
#if 0
    UEYE_CAMERA_INFO_STRUCT _DEVICE_INFO deviceInfo;
    memset(&deviceInfo, 0, sizeof(IS_DEVICE_INFO));
    ueyeTrace(is_DeviceInfo (leftCamera.mCamera, IS_DEVICE_INFO_CMD_GET_DEVICE_INFO, &deviceInfo, sizeof(deviceInfo)), "UEyeCaptureInterface::getDeviceSerial");
#endif

    std::string toReturn = "unknown";

    int camNum = 0;
    ueyeTrace(is_GetNumberOfCameras (&camNum), "UEyeCaptureInterface::getDeviceSerial::is_GetNumberOfCameras");
    UEYE_CAMERA_LIST *camList = (UEYE_CAMERA_LIST *)malloc(sizeof(ULONG) + camNum * sizeof(UEYE_CAMERA_INFO));
    camList->dwCount = camNum;
    ueyeTrace(is_GetCameraList(camList), "UEyeCaptureInterface::getDeviceSerial::is_GetNumberOfCameras");

    for (int i = 0; i < camNum; i++)
    {
        SYNC_PRINT(("UEyeCaptureInterface::getDeviceSerial():devices <%d> <%d>\n", camList->uci[i].dwDeviceID, leftCamera.deviceID));
        if (camList->uci[i].dwDeviceID == leftCamera.deviceID)
        {
            toReturn = std::string(camList->uci[i].SerNo);
            break;
        }
    }
    free(camList);
    SYNC_PRINT(("UEyeCaptureInterface::getDeviceSerial():returning <%s>\n", toReturn.c_str()));
    return toReturn;
}


void UEyeCaptureInterface::SpinThread::run()
{
    qDebug("UEyeCaptureInterface::SpinThread(): New frame thread running");
    qDebug("UEyeCaptureInterface::SpinThread(): Left  camera inited %d", capInterface-> leftCamera.inited);
    qDebug("UEyeCaptureInterface::SpinThread(): Right camera inited %d", capInterface->rightCamera.inited);

    while (capInterface->spinRunning.tryLock()) {
        int result;
        INT timeout = 4000;


//    	usleep(20000);
        if (capInterface->sync == SOFT_SYNC || capInterface->sync == FRAME_HARD_SYNC) {
//            printf("Both cameras fire command\n");
            ueyeTrace(is_FreezeVideo (capInterface->leftCamera .mCamera, IS_DONT_WAIT), "SpinThread::run():is_FreezeVideo");
/*            if (capInterface->rightCamera.inited)
                ueyeTrace(is_FreezeVideo (capInterface->rightCamera.mCamera, IS_DONT_WAIT), "SpinThread::run():is_FreezeVideo");*/

        }

        result = IS_SUCCESS;
        if ((result = capInterface->leftCamera .waitUEyeFrameEvent(timeout)) != IS_SUCCESS)
        {
            SYNC_PRINT(("WaitFrameEvent failed for left camera\n"));
            ueyeTrace(result);
        }
        DEEP_TRACE(("SpinThread::run():Got left frame\n"));



        if (capInterface->sync == SOFT_SYNC || capInterface->sync == FRAME_HARD_SYNC) {
            printf("Both cameras fire command\n");
/*            ueyeTrace(is_FreezeVideo (capInterface->leftCamera .mCamera, IS_DONT_WAIT), "SpinThread::run():is_FreezeVideo");*/
            if (capInterface->rightCamera.inited)
                ueyeTrace(is_FreezeVideo (capInterface->rightCamera.mCamera, IS_DONT_WAIT), "SpinThread::run():is_FreezeVideo");
        }


        if (capInterface->rightCamera.inited)
        {
            result = IS_SUCCESS;
            if ((result = capInterface->rightCamera.waitUEyeFrameEvent(timeout)) != IS_SUCCESS)
            {
                SYNC_PRINT(("WaitFrameEvent failed for right camera\n"));
    //            ueyeTrace(result);
            }
        }
        DEEP_TRACE(("SpinThread::run():Got right frame\n"));



        /* If we are here seems like both new cameras produced frames*/

        int bufIDL = -1;
        int bufIDR = -1;
        char *rawBufferLeft  = NULL;
        char *rawBufferRight = NULL;
        HIDS mCameraLeft;
        HIDS mCameraRight;

        mCameraLeft  = capInterface->leftCamera .mCamera;
        mCameraRight = capInterface->rightCamera.mCamera;

        ueyeTrace(is_GetActSeqBuf(mCameraLeft, &bufIDL, NULL, &rawBufferLeft), "is_GetActSeqBuf for leftcam");
        ueyeTrace(is_LockSeqBuf  (mCameraLeft, IS_IGNORE_PARAMETER, rawBufferLeft), "is_LockSeqBuf for leftcam");

        if (capInterface->rightCamera.inited) {

            ueyeTrace(is_GetActSeqBuf(mCameraRight, &bufIDR, NULL, &rawBufferRight), "is_GetActSeqBuf for right");
            ueyeTrace(is_LockSeqBuf  (mCameraRight, IS_IGNORE_PARAMETER, rawBufferRight), "is_LockSeqBuf for rightcam");
        }

        DEEP_TRACE(("SpinThread::run():We have locked buffers [%d and %d]\n", bufIDL, bufIDR));

        /* Now exchange the buffer that is visible from */
        capInterface->protectFrame.lock();
            UEYEIMAGEINFO imageInfo;

            if (capInterface->currentLeft) {
                ueyeTrace(is_UnlockSeqBuf (mCameraLeft, IS_IGNORE_PARAMETER, (char *)capInterface->currentLeft->buffer), "is_UnlockSeqBuf for left cam");
            }
            ueyeTrace(is_GetImageInfo (mCameraLeft, bufIDL, &imageInfo, sizeof(UEYEIMAGEINFO)), "is_GetImageInfo for left cam");
            capInterface->currentLeft = capInterface->leftCamera.getDescriptorByAddress(rawBufferLeft);
            capInterface->currentLeft->internalTimestamp = imageInfo.u64TimestampDevice;
            capInterface->currentLeft->pcTimestamp = imageInfo.TimestampSystem;

            if (capInterface->rightCamera.inited)
            {
                if (capInterface->currentRight)
                    is_UnlockSeqBuf (mCameraRight, IS_IGNORE_PARAMETER, (char *)capInterface->currentRight->buffer);
                is_GetImageInfo (mCameraRight, bufIDR, &imageInfo, sizeof(UEYEIMAGEINFO));
                capInterface->currentRight = capInterface->rightCamera.getDescriptorByAddress(rawBufferRight);
                capInterface->currentRight->internalTimestamp = imageInfo.u64TimestampDevice;
                capInterface->currentRight->pcTimestamp = imageInfo.TimestampSystem;
            }

            capInterface->skippedCount++;

            capInterface->triggerSkippedCount = is_CameraStatus (mCameraRight, IS_TRIGGER_MISSED, IS_GET_STATUS);
        capInterface->protectFrame.unlock();
       /* For statistics */
        if (capInterface->lastFrameTime.usecsTo(PreciseTimer()) != 0)
        {
           capInterface->frameDelay = capInterface->lastFrameTime.usecsToNow();
        }
        capInterface->lastFrameTime = PreciseTimer::currentTime();



        frame_data_t frameData;
        if (capInterface->rightCamera.inited) {
            frameData.timestamp = (capInterface->currentLeft->usecsTimeStamp() / 2) + (capInterface->currentRight->usecsTimeStamp() / 2);
        } else {
            frameData.timestamp = capInterface->currentLeft->usecsTimeStamp();
        }

        capInterface->notifyAboutNewFrame(frameData);

        capInterface->spinRunning.unlock();
        if (capInterface->shouldStopSpinThread)
        {
            qDebug("Break command received");

            break;
        }
    }
    qDebug("new frame thread finished");
}

void UEyeCaptureInterface::decodeData(UEyeCameraDescriptor *camera, BufferDescriptorType *buffer, G12Buffer **output)
{
    if (!camera->inited) {
        // SYNC_PRINT(("UEyeCaptureInterface::decodeData(0x%0X,,)/n", camera));
        *output = new G12Buffer(100, 100);
        return;
    }

    *output = new G12Buffer(camera->bufferProps.height, camera->bufferProps.width, false);
    for (int i = 0; i < camera->bufferProps.height; i++)
    {
        uint8_t *lineIn  = ((uint8_t *)buffer->buffer) + camera->bufferProps.width * i * camera->bufferProps.bitspp / 8;

        uint16_t *lineOut = &((*output)->element(i,0));

        for (int j = 0; j < camera->bufferProps.width; j++)
        {
            uint16_t value = *(uint16_t *)lineIn;
            //printf("%u\n", value);

            //value = (j % 256) | (i % 1024);
            //value >>= 4;
            *lineOut = value >> 4;
            lineOut++;
            lineIn += camera->bufferProps.bitspp / 8;
        }
    }
}

void UEyeCaptureInterface::decodeData24(UEyeCameraDescriptor *camera, BufferDescriptorType *buffer, RGB24Buffer **output)
{
    if (!camera->inited) {
        //SYNC_PRINT(("UEyeCaptureInterface::decodeData24(0x%0X,,)\n", camera));
        *output = new RGB24Buffer(100, 100);
        return;
    }

    *output = new RGB24Buffer(camera->bufferProps.height, camera->bufferProps.width, false);
    for (int i = 0; i < camera->bufferProps.height; i++)
    {
        uint8_t *lineIn  = ((uint8_t *)buffer->buffer) + camera->bufferProps.width * i * camera->bufferProps.bitspp / 8;

        RGBColor *lineOut = &((*output)->element(i,0));

        for (int j = 0; j < camera->bufferProps.width; j++)
        {
            uint8_t r = lineIn[0];
            uint8_t g = lineIn[1];
            uint8_t b = lineIn[2];

            //printf("%u\n", value);

            //value = (j % 256) | (i % 1024);
            //value >>= 4;
            *lineOut = RGBColor(r,g,b);
            lineOut++;
            lineIn += camera->bufferProps.bitspp / 8;
        }
    }
}

ImageCaptureInterface::CapErrorCode UEyeCaptureInterface::initCapture()
{
    SYNC_PRINT(("UEyeCaptureInterface::initCapture(): called\n"));
    SYNC_PRINT(("UEyeCaptureInterface::initCapture(): Left Camera\n"));
    int resR = 1;
    int resL = 1;
    resL = leftCamera .initBuffer();
    if (rightCamera.inited) {
        SYNC_PRINT(("UEyeCaptureInterface::initCapture(): Right Camera\n"));
        resR = rightCamera.initBuffer();
    } else {
        SYNC_PRINT(("UEyeCaptureInterface::initCapture(): Not initing right Camera\n"));
    }
#if 0
    /* If only one camera started, we assume it is the left camera */
    if (resL && !resR)
    {
        UEyeCameraDescriptor tmp;
        tmp = leftCamera;
        left = rightCamera;
        right = tmp;
    }
#endif

    CapErrorCode result = (CapErrorCode)((bool) resL + (bool) resR);

    SYNC_PRINT(("UEyeCaptureInterface::initCapture(): returning %d\n", result));
    return result;
}

/*ImageCaptureInterface::CapErrorCode UEyeCaptureInterface::queryCameraParameters(CameraParameters &params)
{
    Q_UNUSED(params);
    return ImageCaptureInterface::CapFailure;
}*/


ImageCaptureInterface::CapErrorCode UEyeCaptureInterface::queryCameraParameters(CameraParameters &params)
{
    /* Exposure */
    double defaultExp;
    double minExp;
    double maxExp;
    double stepExp;

    ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_GET_EXPOSURE_DEFAULT, (void*)&defaultExp, sizeof(double)), "is_Exposure:IS_EXPOSURE_CMD_GET_EXPOSURE_DEFAULT");
    ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN, (void*)&minExp,   sizeof(double)), "is_Exposure:IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN");
    ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX, (void*)&maxExp,   sizeof(double)), "is_Exposure:IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX");
    ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_INC, (void*)&stepExp,  sizeof(double)), "is_Exposure:IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_INC");

    CaptureParameter *param = &(params.mCameraControls[CameraParameters::EXPOSURE]);
    param->setActive(true);
    param->setDefaultValue(defaultExp * EXPOSURE_SCALER);
    param->setMinimum     (minExp     * EXPOSURE_SCALER);
    param->setMaximum     (maxExp     * EXPOSURE_SCALER);
    param->setStep        (stepExp    * EXPOSURE_SCALER);

    /* Exposure auto*/
    param = &(params.mCameraControls[CameraParameters::EXPOSURE_AUTO]);

    *param = CaptureParameter();
    param->setActive(true);
    param->setDefaultValue(0);
    param->setMinimum(0);
    param->setMaximum(1);
    param->setStep(1);
    param->setIsMenu(true);
    param->pushMenuItem(QString("False"), 0);
    param->pushMenuItem(QString("True") , 1);


    /* Gain */
    INT defaultGain = is_SetHardwareGain (leftCamera.mCamera, IS_GET_MASTER_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
    param = &(params.mCameraControls[CameraParameters::GAIN]);
    *param = CaptureParameter();
    param->setActive(true);
    param->setDefaultValue(defaultGain);
    param->setMinimum     (0);
    param->setMaximum     (100);
    param->setStep        (1);

    /*Gain Auto*/
    param = &(params.mCameraControls[CameraParameters::GAIN_AUTO]);
    *param = CaptureParameter();
    param->setActive(true);
    param->setDefaultValue(0);
    param->setMinimum(0);
    param->setMaximum(1);
    param->setStep(1);
    param->setIsMenu(true);
    param->pushMenuItem(QString("False"), 0);
    param->pushMenuItem(QString("True") , 1);


    /* Sensor clock */
    UINT defaultClock;
    UINT clockRange[3];

    //ueyeTrace(is_PixelClock(leftCamera.mCamera, IS_PIXELCLOCK_CMD_GET_NUMBER, (void*)&clockSteps, sizeof(clockSteps)));
    //SYNC_PRINT(("UEyeCaptureInterface::queryCameraParameters(): Clock steps: %d", clockSteps));
    ueyeTrace(is_PixelClock(leftCamera.mCamera, IS_PIXELCLOCK_CMD_GET_RANGE  , &clockRange  , sizeof(clockRange)), "is_PixelClock::IS_PIXELCLOCK_CMD_GET_RANGE");
    ueyeTrace(is_PixelClock(leftCamera.mCamera, IS_PIXELCLOCK_CMD_GET_DEFAULT, &defaultClock, sizeof(defaultClock))    , "is_PixelClock::IS_PIXELCLOCK_CMD_GET_DEFAULT");

    param = &(params.mCameraControls[CameraParameters::PIXEL_CLOCK]);
    *param = CaptureParameter();
    param->setActive(true);
    param->setDefaultValue(defaultClock);
    param->setMinimum     (clockRange[0]);
    param->setMaximum     (clockRange[1]);
    param->setStep        (clockRange[2]);

    /* Frame rate */

    double minDFps = 1.0 / 50.0;
    double maxDFps = 1.0 / 1.0;
    double intervalDFps = 1.0 / 50.0;

    ueyeTrace(is_GetFrameTimeRange (leftCamera.mCamera, &minDFps, &maxDFps, &intervalDFps), "is_GetFrameTimeRange:");

    param = &(params.mCameraControls[CameraParameters::FPS]);
    *param = CaptureParameter();
    param->setActive(true);
    param->setDefaultValue(25 * FPS_SCALER);
    param->setMinimum     ((1 / maxDFps)  * FPS_SCALER);
    param->setMaximum     ((1 / minDFps) * FPS_SCALER);
    param->setStep        (1);


    param = &(params.mCameraControls[CameraParameters::MASTER_FLASH_DELAY]);
    param->setActive      (true);
    param->setDefaultValue(0);
    param->setMinimum     (-500000);
    param->setMaximum     (500000);
    param->setStep        (1);

    param = &(params.mCameraControls[CameraParameters::MASTER_FLASH_DURATION]);
    param->setActive      (true);
    param->setDefaultValue(0);
    param->setMinimum     (0);
    param->setMaximum     (500000);
    param->setStep        (1);

    param = &(params.mCameraControls[CameraParameters::SLAVE_TRIGGER_DELAY]);
    param->setActive      (true);
    param->setDefaultValue(0);
    param->setMinimum     (is_SetTriggerDelay (rightCamera.mCamera, IS_GET_MIN_TRIGGER_DELAY));
    param->setMaximum     (is_SetTriggerDelay (rightCamera.mCamera, IS_GET_MAX_TRIGGER_DELAY));
    param->setStep        (is_SetTriggerDelay (rightCamera.mCamera, IS_GET_TRIGGER_DELAY_GRANULARITY));


    return ImageCaptureInterface::SUCCESS;
}

UEyeCaptureInterface::CapErrorCode UEyeCaptureInterface::setCaptureProperty(int id, int value)
{
    int retL = 0;
    int retR = 0;

    switch (id)
    {
        case (CameraParameters::EXPOSURE):
        {
            double ms = 0.0;
            ms = (double)value / EXPOSURE_SCALER;
            ueyeTrace(is_Exposure (leftCamera.mCamera, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&ms, sizeof(double)), "is_Exposure::IS_EXPOSURE_CMD_SET_EXPOSURE");
            if (rightCamera.inited) {
                ms = (double)value / EXPOSURE_SCALER;
                ueyeTrace(is_Exposure (rightCamera.mCamera, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&ms, sizeof(double)), "is_Exposure::IS_EXPOSURE_CMD_SET_EXPOSURE");
            }
            return SUCCESS;
        }
        case (CameraParameters::GAIN):
        {
            ueyeTrace(is_SetHardwareGain(leftCamera.mCamera, value, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER));
            if (rightCamera.inited) {
                ueyeTrace(is_SetHardwareGain(rightCamera.mCamera, value, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER));
            }
            return SUCCESS;
        }

        case (CameraParameters::FPS):
        {
            double fps = (double)value / FPS_SCALER;
            double newFps;
            ueyeTrace(is_SetFrameRate (leftCamera.mCamera , fps, &newFps), "left: is_SetFrameRate");
            if (rightCamera.inited) {
                ueyeTrace(is_SetFrameRate (rightCamera.mCamera, fps, &newFps), "right: is_SetFrameRate");
            }
            return SUCCESS;
        }

        case (CameraParameters::PIXEL_CLOCK):
        {

            UINT newClock = value;
            ueyeTrace(is_PixelClock (leftCamera.mCamera , IS_PIXELCLOCK_CMD_SET, (void *)&newClock, sizeof(newClock)), "left is_PixelClock::IS_PIXELCLOCK_CMD_SET");
            if (rightCamera.inited) {
                ueyeTrace(is_PixelClock (rightCamera.mCamera, IS_PIXELCLOCK_CMD_SET, (void *)&newClock, sizeof(newClock)), "right is_PixelClock::IS_PIXELCLOCK_CMD_SET");
            }
            return SUCCESS;
        }

        case (CameraParameters::MASTER_FLASH_DELAY):
        {
            IO_FLASH_PARAMS flashParams;
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 0;
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_GET_PARAMS, (void*)&flashParams, sizeof(flashParams)));            
            flashParams.s32Delay = value;
            if (rightCamera.inited) {
                ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
            }
            return SUCCESS;
        }
        case (CameraParameters::MASTER_FLASH_DURATION):
        {
            IO_FLASH_PARAMS flashParams;
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 0;
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_GET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
            flashParams.u32Duration = value;
            if (rightCamera.inited) {
                ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
            }
            return SUCCESS;
        }

        case (CameraParameters::SLAVE_TRIGGER_DELAY):
        {
            if (rightCamera.inited) {
                is_SetTriggerDelay (rightCamera.mCamera, value);
            }
            return SUCCESS;
        }
        case (CameraParameters::EXPOSURE_AUTO) :
        {
            double enable;
            enable = value;
            ueyeTrace(is_SetAutoParameter(leftCamera.mCamera, IS_SET_ENABLE_AUTO_SHUTTER, &enable, 0));
            if (rightCamera.inited) {
                ueyeTrace(is_SetAutoParameter(rightCamera.mCamera, IS_SET_ENABLE_AUTO_SHUTTER, &enable, 0));
            }
            return SUCCESS;
        }
        case (CameraParameters::GAIN_AUTO) :
        {
            double enable = value;
            ueyeTrace(is_SetAutoParameter(leftCamera.mCamera, IS_SET_ENABLE_AUTO_GAIN, &enable, 0));
            if (rightCamera.inited) {
                ueyeTrace(is_SetAutoParameter(leftCamera.mCamera, IS_SET_ENABLE_AUTO_GAIN, &enable, 0));
            }
            return SUCCESS;
        }
        default:
        {
            printf("UEYE: Warning: Set request for unknown parameter %s (%d)\n", CameraParameters::getName((CameraParameters::CameraControls) id), id);
            return ImageCaptureInterface::FAILURE;
        }
    }

    return (ImageCaptureInterface::CapErrorCode) ((bool) retL + (bool) retR);
}

ImageCaptureInterface::CapErrorCode UEyeCaptureInterface::getCaptureProperty(int id, int *value)
{
 //   printf("Getting the values of capture properties is not supported for uEye interface yet.\n");

    switch (id)
    {
        case (CameraParameters::EXPOSURE):
        {
            double ms = 0.0;
            ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&ms, sizeof(double)));
            *value = ms * EXPOSURE_SCALER;
            return SUCCESS;
        }
        case (CameraParameters::GAIN):
        {
            ueyeTrace(*value = is_SetHardwareGain(leftCamera.mCamera , IS_GET_MASTER_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER ));
            return SUCCESS;
        }
        case (CameraParameters::EXPOSURE_AUTO) :
        {
            double autoExp = 0.0;
            double maxExp  = 0.0;

            ueyeTrace(is_SetAutoParameter(leftCamera.mCamera, IS_GET_ENABLE_AUTO_SHUTTER, &autoExp, 0));
            ueyeTrace(is_SetAutoParameter(leftCamera.mCamera, IS_GET_AUTO_SHUTTER_MAX, &maxExp, 0));
            printf("ImageCaptureInterface::CapErrorCode UEyeCaptureInterface::getCaptureProperty(): %lf %lf\n", autoExp, maxExp);
            if (maxExp != 0) {
                *value = 2;
                return SUCCESS;
            }
            *value =  (autoExp != 0.0) ? 1 : 0;
            return SUCCESS;
        }
        case (CameraParameters::GAIN_AUTO) :
        {
            double autoGain = 0.0;

            ueyeTrace(is_SetAutoParameter(leftCamera.mCamera, IS_GET_ENABLE_AUTO_GAIN, &autoGain, 0));
            *value = (autoGain != 0.0) ? 1 : 0;
            return SUCCESS;
        }

       case (CameraParameters::FPS):
       {
           double newFps;
           ueyeTrace(is_SetFrameRate (leftCamera.mCamera, IS_GET_FRAMERATE, &newFps));
           *value = newFps * FPS_SCALER;
           return SUCCESS;
       }

       case (CameraParameters::PIXEL_CLOCK):
       {
           UINT newClock;
           ueyeTrace(is_PixelClock(leftCamera.mCamera, IS_PIXELCLOCK_CMD_GET, (void*)&newClock, sizeof(newClock)));
           *value = newClock;
           return SUCCESS;
       }

       case (CameraParameters::MASTER_FLASH_DELAY):
       {
           IO_FLASH_PARAMS flashParams;
           flashParams.s32Delay    = 0;
           flashParams.u32Duration = 0;
           ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_GET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
           *value = flashParams.s32Delay;
           return SUCCESS;
       }
       case (CameraParameters::MASTER_FLASH_DURATION):
       {
           IO_FLASH_PARAMS flashParams;
           flashParams.s32Delay    = 0;
           flashParams.u32Duration = 0;
           ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_GET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
           *value = flashParams.u32Duration;
           return SUCCESS;
       }

       case (CameraParameters::SLAVE_TRIGGER_DELAY):
       {
           *value = is_SetTriggerDelay (rightCamera.mCamera, IS_GET_TRIGGER_DELAY);
           return SUCCESS;
       }


       default:
       {
           printf("UEYE: Get request for unknown parameter %s(%d)\n", CameraParameters::getName((CameraParameters::CameraControls) id), id);
           return ImageCaptureInterface::FAILURE;
       }
    }

    return FAILURE;
}
