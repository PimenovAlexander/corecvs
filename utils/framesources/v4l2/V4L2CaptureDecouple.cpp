/*
 * V4l2CaptureDecouple.cpp
 *
 *  Created on: Jul 21, 2012
 *      Author: alexander
 */

#include <QtCore/QRegExp>

#include "V4L2CaptureDecouple.h"
#include "decoupleYUYV.h"

V4L2CaptureDecoupleInterface::V4L2CaptureDecoupleInterface(string _devname)
    : spin(this)
{
    this->devname = _devname;

    //     Group Number                   1       2 3      4       56        7       8         9 10     11     1213    14
    QRegExp deviceStringPattern(QString("^([^,:]*)(:(\\d*)/(\\d*))?((:mjpeg)|(:yuyv)|(:fjpeg))?(:(\\d*)x(\\d*))?((:rc)|(:rc2)|(:sbs)|(:rcf)|(:sc1))?$"));
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
        exit(EXIT_FAILURE);
    }
    printf (
        "Parsed data:\n"
        "  | - Device =%s\n"
        "  | - FPS %s/%s\n"
        "  | - Size [%sx%s]\n"
        "  | - Compressing: %s\n"
        "  \\ - Coupling: <%s>\n",
        deviceStringPattern.cap(DeviceGroup).toLatin1().constData(),
        deviceStringPattern.cap(FpsNumGroup).toLatin1().constData(),
        deviceStringPattern.cap(FpsDenumGroup).toLatin1().constData(),
        deviceStringPattern.cap(WidthGroup).toLatin1().constData(),
        deviceStringPattern.cap(HeightGroup).toLatin1().constData(),
        deviceStringPattern.cap(CompressionGroup).toLatin1().constData(),
        deviceStringPattern.cap(CouplingGroup).toLatin1().constData()
    );

    deviceName =  deviceStringPattern.cap(DeviceGroup).toLatin1().constData();

    bool err;
    cameraMode.fpsnum = deviceStringPattern.cap(FpsNumGroup).toInt(&err);
    if (!err) cameraMode.fpsnum = 1;

    cameraMode.fpsdenum = deviceStringPattern.cap(FpsDenumGroup).toInt(&err);
    if (!err) cameraMode.fpsdenum = 10;

    cameraMode.width = deviceStringPattern.cap(WidthGroup).toInt(&err);
    if (!err || cameraMode.width <= 0) cameraMode.width = 800;

    cameraMode.height = deviceStringPattern.cap(HeightGroup).toInt(&err);
    if (!err || cameraMode.height <= 0) cameraMode.height = 600;

    decoder = V4L2CaptureInterface::UNCOMPRESSED;
    if (!deviceStringPattern.cap(CompressionGroup).isEmpty())
    {
        if (!deviceStringPattern.cap(CompressionGroup).compare(QString(":mjpeg")))
            decoder = V4L2CaptureInterface::COMPRESSED_JPEG;
        if (!deviceStringPattern.cap(CompressionGroup).compare(QString(":fjpeg")))
            decoder = V4L2CaptureInterface::COMPRESSED_FAST_JPEG;
    }

    cameraMode.compressed = (decoder != V4L2CaptureInterface::UNCOMPRESSED);

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
    if (!deviceStringPattern.cap(CouplingGroup).compare(QString(":sc1"))) {
        coupling = DecoupleYUYV::SIDEBYSIDE_SYNCCAM_1;
    }

    printf("Capture device: V4L2 %s\n", deviceName.c_str());
    printf("MJPEG compression is: %s\n", V4L2CaptureInterface::CODEC_NAMES[decoder]);
    printf("Coupling is: %d\n", coupling);
}


V4L2CaptureDecoupleInterface::FramePair V4L2CaptureDecoupleInterface::getFrame()
{
    CaptureStatistics  stats;

    PreciseTimer start = PreciseTimer::currentTime();
    FramePair result;

    protectFrame.lock();
        uint8_t *ptr = (uint8_t*)(camera.buffers[current.index].start);

        DecoupleYUYV::decouple(formatH, formatW, ptr, coupling, result);

        if (current.isFilled) {
            result.timeStampLeft = result.timeStampRight = current.usecsTimeStamp();
        }

        if (skippedCount == 0) {
            SYNC_PRINT(("Warning: Requested same frames twice. Is this by design?\n"));
        }

        stats.framesSkipped = skippedCount > 0 ? skippedCount - 1 : 0;
        skippedCount = 0;
    protectFrame.unlock();

    stats.values[CaptureStatistics::DECODING_TIME]    = start.usecsToNow();
    stats.values[CaptureStatistics::INTERFRAME_DELAY] = frameDelay;

    stats.values[CaptureStatistics::DESYNC_TIME] = 0;
    stats.values[CaptureStatistics::DATA_SIZE]   = current.bytesused;

    emit newStatisticsReady(stats);

    return result;
}


ImageCaptureInterface::CapErrorCode V4L2CaptureDecoupleInterface::initCapture()
{
    camera.initCamera(deviceName, cameraMode);

    formatH = camera.formatH;
    formatW = camera.formatW;

    skippedCount = 0;
    frameDelay = 0;
    shouldStopSpinThread = false;

    int res = camera.initBuffers(LoopQueueBufferNumber, LoopQueueBufferNumberMin);

    return (res == 0) ? SUCCESS : FAILURE;
}

ImageCaptureInterface::CapErrorCode V4L2CaptureDecoupleInterface::startCapture()
{
    int res = camera.start();

    setCaptureProperty(CameraParameters::SATURATION, 0);

    shouldStopSpinThread = false;
    spin.start();

    return (res == 0) ? SUCCESS : FAILURE;
}



/* TODO: Process errors of queue/dequeue */
void V4L2CaptureDecoupleInterface::SpinThread::run()
{
    while (mInterface->spinRunning.tryLock())
    {
        V4L2BufferDescriptor newBuffer;

        V4L2CameraDescriptor* camera = &(mInterface->camera);

        V4L2BufferDescriptor* current = &(mInterface->current);

        /* First we block till we will get a new frame */
        /*TODO: Wonder if dequeue should be static */
        /*int result =*/ camera->dequeue(newBuffer);

        /**
         *  Now we have two new buffers
         *  So we do some thread protected operations.
         *  1. Enqueue the previously used buffers
         *  2. Replace them with the new ones
         **/
        mInterface->protectFrame.lock();
        {
            camera->enqueue(*current);
            *current = newBuffer;
            mInterface->skippedCount++;
        }
        mInterface->protectFrame.unlock();

        /* For statistics */
        if (mInterface->lastFrameTime.usecsTo(PreciseTimer()) != 0)
        {
            mInterface->frameDelay = mInterface->lastFrameTime.usecsToNow();
        }
        mInterface->lastFrameTime = PreciseTimer::currentTime();

        frame_data_t frameData;
        frameData.timestamp = current->usecsTimeStamp();
        mInterface->notifyAboutNewFrame(frameData);

        mInterface->spinRunning.unlock();
        if (mInterface->shouldStopSpinThread)
        {
            break;
        }
    }
}

V4L2CaptureDecoupleInterface::CapErrorCode V4L2CaptureDecoupleInterface::setCaptureProperty(int id, int value)
{
    camera.setCaptureProperty(id, value);
    return SUCCESS;
}

V4L2CaptureDecoupleInterface::CapErrorCode V4L2CaptureDecoupleInterface::getCaptureProperty(int id, int * value)
{
    camera.getCaptureProperty(id, value);
    return SUCCESS;
}

V4L2CaptureDecoupleInterface::CapErrorCode V4L2CaptureDecoupleInterface::queryCameraParameters(CameraParameters & parameter)
{
    camera.queryCameraParameters(parameter);
    return SUCCESS;
}

QString V4L2CaptureDecoupleInterface::getInterfaceName()
{
    return QString("v4l2d:") + QString(devname.c_str());
}

V4L2CaptureDecoupleInterface::~V4L2CaptureDecoupleInterface()
{
   // spinRunning.lock();
    cout << "V4L2CaptureDecoupleInterface::Request for killing the thread" << endl;

    shouldStopSpinThread = true;
    bool result = spinRunning.tryLock(1000);

    if (result)
        printf("V4L2CaptureDecoupleInterface::Camera thread killed\n");
    else
        printf("V4L2CaptureDecoupleInterface::Unable to exit Camera thread\n");

    SYNC_PRINT(("V4L2CaptureDecoupleInterface::Stopping cameras\n"));
    camera.stop();
}
