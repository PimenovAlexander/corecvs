#include <cstdlib>
#include <regex>

#include "abstractFileCapture.h"
#include "core/utils/utils.h"

AbstractFileCapture::AbstractFileCapture(const std::string &params)
    : mDelay(0)
    , mSpin(NULL)
    , mShouldSkipUnclaimed(false)
    , shouldStopSpinThread(false)
    , mTimeStamp(0)
{
    //     Group numbers                  1      2 3      4       5 6         7
    std::regex deviceStringPattern("^([^,]*)(,(\\d*)/(\\d*))?(,(\\d*)ms)?(,skip)?$");
    static const int filenamePatternGroup  = 1;
    static const int fpsNumGroup           = 3;
    static const int fpsDenumGroup         = 4;
    static const int delayGroup            = 6;
    static const int shouldSkipGroup       = 7;

    std::smatch matches;

    SYNC_PRINT(("Input string %s\n", params.c_str()));
    int result = std::regex_match(params, matches, deviceStringPattern);
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
            matches[filenamePatternGroup].str().c_str(),
            matches[fpsNumGroup].str().c_str(),
            matches[fpsDenumGroup].str().c_str(),
            matches[delayGroup].str().c_str(),
            matches[shouldSkipGroup].str().c_str()
        ));

    // store the given path format for further usage
    mPathFmt = matches[filenamePatternGroup];

    bool isOk = false;
    int fpsnum = HelperUtils::parseInt(matches[fpsNumGroup].str(), &isOk);
    if (!isOk || fpsnum < 0)
        fpsnum = -1;

    int fpsdenum = HelperUtils::parseInt(matches[fpsDenumGroup].str(), &isOk);
    if (!isOk || fpsdenum <= 0)
        fpsdenum = -1;

    mDelay = HelperUtils::parseInt(matches[delayGroup].str(), &isOk);
    if (!isOk /*|| mDelay < 0*/)
        mDelay = capDefaultDelay;

    if (fpsnum != -1 && fpsdenum != -1 && fpsdenum != 0)
        mDelay = 1000 * fpsnum / fpsdenum;

    if (matches[shouldSkipGroup].str() == std::string(",skip"))
        mShouldSkipUnclaimed = true;
}

AbstractFileCapture::~AbstractFileCapture()
{
    shouldStopSpinThread = true;
    mSpinThreadRun.lock();
    mSpin->join();
    DOTRACE(("file capture thread killed\n"));
    delete mSpin;
}

ImageCaptureInterface::CapErrorCode AbstractFileCapture::setCaptureProperty(int /*id*/, int /*value*/)
{   
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

std::mutex& AbstractFileCapture::spinThreadRunMutex()
{
    return mSpinThreadRun;
}

std::mutex& AbstractFileCapture::protectFrameMutex()
{
    return mProtectFrame;
}

/*bool AbstractFileCapture::shouldSkipUnclaimed() const
{
    return mShouldSkipUnclaimed;
}*/

void AbstractFileCapture::notifyAboutStreamPaused()
{
    if (imageInterfaceReceiver != NULL) {
        imageInterfaceReceiver->streamPausedCallback();
    } else {
        SYNC_PRINT(("AbstractFileCapture::notifyAboutStreamPaused() : imageInterfaceReceiver is NULL\n"));
    }
}

bool AbstractFileCapture::supportPause()
{
    return true;
}

void AbstractFileCapture::increaseTimeStamp()
{
    mTimeStamp += mDelay * 1000;
}
