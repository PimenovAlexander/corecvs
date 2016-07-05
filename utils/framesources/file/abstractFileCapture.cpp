#include <cstdlib>

#include <QtCore/QRegExp>
#include <QtCore/QString>

#include "abstractFileCapture.h"

AbstractFileCapture::AbstractFileCapture(QString const &params)
    : mDelay(0)
    , mSpin(NULL)
    , mShouldSkipUnclaimed(false)
    , shouldStopSpinThread(false)
    , mTimeStamp(0)
{
    //     Group numbers                  1      2 3      4       5 6         7
    QRegExp deviceStringPattern(QString("^([^,]*)(,(\\d*)/(\\d*))?(,(\\d*)ms)?(,skip)?$"));
    static const int filenamePatternGroup  = 1;
    static const int fpsNumGroup           = 3;
    static const int fpsDenumGroup         = 4;
    static const int delayGroup            = 6;
    static const int shouldSkipGroup       = 7;

    SYNC_PRINT(("Input string %s\n", params.toLatin1().constData()));
    int result = deviceStringPattern.indexIn(params);
    if (result == -1)
    {
        printf("Error in device string format\n");
        return;
    }

    SYNC_PRINT((
        "Parsed data:\n"
        "  | - Filename Pattern: <%s>\n"
        "  | - FPS: <%s/%s>\n"
        "  | - Delay: <%s>\n"
        "  \\ - Skip: <%s>\n",
        deviceStringPattern.cap(filenamePatternGroup).toLatin1().constData(),
        deviceStringPattern.cap(fpsNumGroup)         .toLatin1().constData(),
        deviceStringPattern.cap(fpsDenumGroup)       .toLatin1().constData(),
        deviceStringPattern.cap(delayGroup)          .toLatin1().constData(),
        deviceStringPattern.cap(shouldSkipGroup)     .toLatin1().constData()
        ));

    // store the given path format for further usage
    mPathFmt = deviceStringPattern.cap(filenamePatternGroup).toLatin1().constData();

    bool err = false;
    int fpsnum = deviceStringPattern.cap(fpsNumGroup).toInt(&err);
    if (!err || fpsnum < 0)
        fpsnum = -1;

    int fpsdenum = deviceStringPattern.cap(fpsDenumGroup).toInt(&err);
    if (!err || fpsdenum <= 0)
        fpsdenum = -1;

    mDelay = deviceStringPattern.cap(delayGroup).toInt(&err);
    if (!err /*|| mDelay < 0*/)
        mDelay = capDefaultDelay;

    if (fpsnum != -1 && fpsdenum != -1 && fpsdenum != 0)
        mDelay = 1000 * fpsnum / fpsdenum;

    if (deviceStringPattern.cap(shouldSkipGroup) == QString(",skip"))
        mShouldSkipUnclaimed = true;
}

AbstractFileCapture::~AbstractFileCapture()
{
    shouldStopSpinThread = true;
    mSpinThreadRun.lock();
    mSpin->wait();
    DOTRACE(("file capture thread killed\n"));
    delete mSpin;
}

ImageCaptureInterface::CapErrorCode AbstractFileCapture::setCaptureProperty(int id, int value)
{
    Q_UNUSED(id);
    Q_UNUSED(value);
    return ImageCaptureInterface::FAILURE;
}

ImageCaptureInterface::CapErrorCode AbstractFileCapture::initCapture()
{
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode AbstractFileCapture::startCapture()
{
    mSpin->start();
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode AbstractFileCapture::pauseCapture()
{
    mSpin->togglePause();
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode AbstractFileCapture::nextFrame()
{
    mSpin->nextFrame();
    return ImageCaptureInterface::SUCCESS;
}

QMutex& AbstractFileCapture::spinThreadRunMutex()
{
    return mSpinThreadRun;
}

QMutex& AbstractFileCapture::protectFrameMutex()
{
    return mProtectFrame;
}

/*bool AbstractFileCapture::shouldSkipUnclaimed() const
{
    return mShouldSkipUnclaimed;
}*/

void AbstractFileCapture::notifyAboutStreamPaused()
{
    emit streamPaused();
}

bool AbstractFileCapture::supportPause()
{
    return true;
}

void AbstractFileCapture::increaseTimeStamp()
{
    mTimeStamp += mDelay * 1000;
}
