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

#include "global.h"

#include "uEyeCapture.h"
#include "preciseTimer.h"


#ifdef PROFILE_DEQUEUE
#define TRACE_DEQUEUE(X) printf X
#else
#define TRACE_DEQUEUE(X)
#endif

/*static*/ const double UEyeCaptureInterface::EXPOSURE_SCALER = 10.0;
/*static*/ const double UEyeCaptureInterface::FPS_SCALER      = 100.0;

int UEyeCaptureInterface::ueyeTrace(int result)
{
    if (result != IS_SUCCESS)
    {
        char * str = 0;
        is_GetError(1,&result,&str);
        printf("   UEYE: Function failed with error code %d. Error string: %s\n", result, str);
    }

    return result;
}




UEyeCaptureInterface::UEyeCaptureInterface(string _devname) :
    sync(NO_SYNC),
    currentLeft(NULL),
    currentRight(NULL),
    spin(this)
{
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

    leftCamera.init(leftID, (binning == 2), globalShutter, pixelClock, fps);
    rightCamera.init(rightID, (binning == 2), globalShutter, pixelClock, fps);

    skippedCount = 0;
    triggerSkippedCount = 0;
    frameDelay = 0;
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

        result.leftTimeStamp  = currentLeft->usecsTimeStamp();
        result.rightTimeStamp = currentRight->usecsTimeStamp();

        stats.framesSkipped = skippedCount > 0 ? skippedCount - 1 : 0;
        skippedCount = 0;

        stats.triggerSkipped = triggerSkippedCount;
        triggerSkippedCount = 0;

        int64_t internalDesync =  currentLeft->internalTimestamp - currentRight->internalTimestamp;
    protectFrame.unlock();

    stats.values[CaptureStatistics::DECODING_TIME] = start.usecsToNow();
    stats.values[CaptureStatistics::INTERFRAME_DELAY] = frameDelay;

    int64_t desync =  result.leftTimeStamp - result.rightTimeStamp;
    stats.values[CaptureStatistics::DESYNC_TIME] = desync > 0 ? desync : -desync;
    stats.values[CaptureStatistics::INTERNAL_DESYNC_TIME] = internalDesync > 0 ? internalDesync : -internalDesync;

    /* Get temperature data */
    stats.temperature[0] = leftCamera.getTemperature();
    stats.temperature[1] = rightCamera.getTemperature();

    //stats.values[CaptureStatistics::DATA_SIZE] = currentLeft.bytesused;

    emit newStatisticsReady(stats);

//    printf("Finished getFrame\n");
    return result;
}

UEyeCaptureInterface::~UEyeCaptureInterface()
{
    cout << "Request for killing the thread" << endl;
    shouldStopSpinThread = true;
    bool result = spinRunning.tryLock(1000);

    is_DisableEvent(leftCamera.mCamera, IS_SET_EVENT_FRAME);
    is_DisableEvent(rightCamera.mCamera, IS_SET_EVENT_FRAME);

#ifdef Q_OS_WIN
    is_ExitEvent (leftCamera .mCamera, IS_SET_EVENT_FRAME);
    is_ExitEvent (rightCamera.mCamera, IS_SET_EVENT_FRAME);

    CloseHandle(leftCamera .mWinEvent);
    CloseHandle(rightCamera.mWinEvent);
#endif

    if (result)
        printf("Camera thread killed\n");
    else
        printf("Unable to exit Camera thread\n");

    printf("Deleting image buffers\n");

    leftCamera.deAllocImages();
    rightCamera.deAllocImages();

    printf("uEye: Closing camera...");
    is_ExitCamera(leftCamera.mCamera);
    is_ExitCamera(rightCamera.mCamera);
    printf("done\n");

}

ImageCaptureInterface::CapErrorCode UEyeCaptureInterface::startCapture()
{
    qDebug("Start capture");

    printf("Enabling events...\n");
#ifdef Q_OS_WIN
    leftCamera.mWinEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    is_InitEvent(leftCamera.mCamera, leftCamera.mWinEvent, IS_SET_EVENT_FRAME);

    rightCamera.mWinEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    is_InitEvent(rightCamera.mCamera, rightCamera.mWinEvent, IS_SET_EVENT_FRAME);
#endif

    is_EnableEvent(leftCamera.mCamera, IS_SET_EVENT_FRAME);
    is_EnableEvent(rightCamera.mCamera, IS_SET_EVENT_FRAME);

    switch (sync)
    {
        case NO_SYNC:
            is_CaptureVideo (leftCamera.mCamera, IS_DONT_WAIT);
            is_CaptureVideo (rightCamera.mCamera, IS_DONT_WAIT);
            break;
        case SOFT_SYNC:
            is_SetExternalTrigger (leftCamera.mCamera , IS_SET_TRIGGER_SOFTWARE);
            is_SetExternalTrigger (rightCamera.mCamera, IS_SET_TRIGGER_SOFTWARE);
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


void UEyeCaptureInterface::SpinThread::run()
{
    qDebug("new frame thread running");
    while (capInterface->spinRunning.tryLock()) {

    	//usleep(20000);
        if (capInterface->sync == SOFT_SYNC || capInterface->sync == FRAME_HARD_SYNC) {
    	   // printf("Both cameras fire!!!\n");
            ueyeTrace(is_FreezeVideo (capInterface->rightCamera.mCamera, IS_DONT_WAIT));
            ueyeTrace(is_FreezeVideo (capInterface->leftCamera .mCamera, IS_DONT_WAIT));
        }

        int result = IS_SUCCESS;

        while ((result = capInterface->rightCamera.waitUEyeFrameEvent(INFINITE)) != IS_SUCCESS)
        {
            SYNC_PRINT(("WaitFrameEvent failed for right camera\n"));
            ueyeTrace(result);
        }
        //SYNC_PRINT(("Got right frame\n"));

        while ((result = capInterface->leftCamera .waitUEyeFrameEvent(INFINITE)) != IS_SUCCESS)
        {
            SYNC_PRINT(("WaitFrameEvent failed for left camera\n"));
            ueyeTrace(result);
        }
        //SYNC_PRINT(("Got left frame\n"));


        /* If we are here seems like both new cameras produced frames*/

        int bufIDL, bufIDR;
        char *rawBufferLeft  = NULL;
        char *rawBufferRight = NULL;
        HIDS mCameraLeft;
        HIDS mCameraRight;

        mCameraLeft = capInterface->leftCamera.mCamera;
        is_GetActSeqBuf(mCameraLeft, &bufIDL, NULL, &rawBufferLeft);
        is_LockSeqBuf (mCameraLeft, IS_IGNORE_PARAMETER, rawBufferLeft);
        mCameraRight = capInterface->rightCamera.mCamera;
        is_GetActSeqBuf(mCameraRight, &bufIDR, NULL, &rawBufferRight);
        is_LockSeqBuf (mCameraRight, IS_IGNORE_PARAMETER, rawBufferRight);

       // SYNC_PRINT(("We have locked buffers [%d and %d]\n", bufIDL, bufIDR));

        /* Now exchange the buffer that is visible from */
        capInterface->protectFrame.lock();
            UEYEIMAGEINFO imageInfo;

            if (capInterface->currentLeft)
                is_UnlockSeqBuf (mCameraLeft, IS_IGNORE_PARAMETER, (char *)capInterface->currentLeft->buffer);
            is_GetImageInfo (mCameraLeft, bufIDL, &imageInfo, sizeof(UEYEIMAGEINFO));
            capInterface->currentLeft = capInterface->leftCamera.getDescriptorByAddress(rawBufferLeft);
            capInterface->currentLeft->internalTimestamp = imageInfo.u64TimestampDevice;
            capInterface->currentLeft->pcTimestamp = imageInfo.TimestampSystem;

            if (capInterface->currentRight)
                is_UnlockSeqBuf (mCameraRight, IS_IGNORE_PARAMETER, (char *)capInterface->currentRight->buffer);
            is_GetImageInfo (mCameraRight, bufIDR, &imageInfo, sizeof(UEYEIMAGEINFO));
            capInterface->currentRight = capInterface->rightCamera.getDescriptorByAddress(rawBufferRight);
            capInterface->currentRight->internalTimestamp = imageInfo.u64TimestampDevice;
            capInterface->currentRight->pcTimestamp = imageInfo.TimestampSystem;

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
        frameData.timestamp = (capInterface->currentLeft->usecsTimeStamp() / 2) + (capInterface->currentRight->usecsTimeStamp() / 2);
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
#if 0
    if (!buffer->isFilled)
    {
        *output = new G12Buffer(formatH, formatW);
        return;
    }
#endif

    *output = new G12Buffer(camera->bufferProps.height, camera->bufferProps.width, false);
    for (int i = 0; i < camera->bufferProps.height; i++)
    {
        uint16_t *lineIn  = ((uint16_t *)buffer->buffer) + camera->bufferProps.width * i;
        uint16_t *lineOut = &((*output)->element(i,0));
        for (int j = 0; j < camera->bufferProps.width; j++)
        {
            uint16_t value = *lineIn;
            //printf("%u\n", value);

            //value = (j % 256) | (i % 1024);
            //value >>= 4;
            *lineOut = value;
            lineOut++;
            lineIn++;
        }
    }

}

ImageCaptureInterface::CapErrorCode UEyeCaptureInterface::initCapture()
{
    int resR = leftCamera .initBuffer();
    int resL = rightCamera.initBuffer();
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
    return (CapErrorCode) ((bool) resL + (bool) resR);
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

    ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_GET_EXPOSURE_DEFAULT, (void*)&defaultExp, sizeof(double)));
    ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MIN, (void*)&minExp,   sizeof(double)));
    ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_MAX, (void*)&maxExp,   sizeof(double)));
    ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE_INC, (void*)&stepExp,  sizeof(double)));

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

    /* Frame rate */
    param = &(params.mCameraControls[CameraParameters::FPS]);
    param->setActive(true);
    param->setDefaultValue(25 * FPS_SCALER);
    param->setMinimum     (0  * FPS_SCALER);
    param->setMaximum     (50 * FPS_SCALER);
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
            ueyeTrace(is_Exposure (leftCamera.mCamera , IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&ms, sizeof(double)));
            ms = (double)value / EXPOSURE_SCALER;
            ueyeTrace(is_Exposure (rightCamera.mCamera, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&ms, sizeof(double)));
            return SUCCESS;
        }
        case (CameraParameters::FPS):
        {
            double fps = (double)value / FPS_SCALER;
            double newFps;
            ueyeTrace(is_SetFrameRate (leftCamera.mCamera , fps, &newFps));
            ueyeTrace(is_SetFrameRate (rightCamera.mCamera, fps, &newFps));
            return SUCCESS;
        }
        case (CameraParameters::MASTER_FLASH_DELAY):
        {
            IO_FLASH_PARAMS flashParams;
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 0;
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_GET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
            flashParams.s32Delay = value;
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
            return SUCCESS;
        }
        case (CameraParameters::MASTER_FLASH_DURATION):
        {
            IO_FLASH_PARAMS flashParams;
            flashParams.s32Delay    = 0;
            flashParams.u32Duration = 0;
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_GET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
            flashParams.u32Duration = value;
            ueyeTrace(is_IO(leftCamera.mCamera, IS_IO_CMD_FLASH_SET_PARAMS, (void*)&flashParams, sizeof(flashParams)));
            return SUCCESS;
        }

        case (CameraParameters::SLAVE_TRIGGER_DELAY):
        {
            is_SetTriggerDelay (rightCamera.mCamera, value);
            return SUCCESS;
        }
        case (CameraParameters::EXPOSURE_AUTO) :
        {
            double enable = value;
            ueyeTrace(is_SetAutoParameter(leftCamera.mCamera, IS_SET_ENABLE_AUTO_SHUTTER, &enable, 0));
            return SUCCESS;
        }
        default:
        {
            printf("Set request for unknown parameter (%d)\n", id);
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

       case (CameraParameters::FPS):
       {
           double newFps;
           ueyeTrace(is_SetFrameRate (leftCamera.mCamera , IS_GET_FRAMERATE, &newFps));
           *value = newFps * FPS_SCALER;
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
           printf("Set request for unknown parameter (%d)\n", id);
           return ImageCaptureInterface::FAILURE;
       }
    }

    return FAILURE;
}
