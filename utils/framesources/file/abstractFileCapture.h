#pragma once

#include <QtCore/QMutex>

#include "imageFileCaptureInterface.h"
#include "abstractFileCaptureSpinThread.h"

/**
    This class is a base for the file interfaces that simulates the camera and skips frames if they are not claimed in time.

**/
class AbstractFileCapture : public ImageFileCaptureInterface
{
public:
    AbstractFileCapture(std::string const &params);
    virtual ~AbstractFileCapture();

    virtual FramePair getFrame() = 0;

    virtual CapErrorCode setCaptureProperty(int id, int value);

    virtual CapErrorCode initCapture();
    virtual CapErrorCode startCapture();
    virtual CapErrorCode pauseCapture();
    virtual CapErrorCode nextFrame();

    virtual bool supportPause();

    QMutex& spinThreadRunMutex();
    QMutex& protectFrameMutex();

    void    notifyAboutStreamPaused();
    void    increaseTimeStamp();

protected:
    static cint capDefaultDelay = 35;

    /** Delay to sleep before querying for a new frame in milliseconds */
    uint64_t    mDelay;
    FramePair   mCurrent;

    //QString   mPathFmt; // moved to ImageFileCaptureInterface
    friend class AbstractFileCaptureSpinThread;
    AbstractFileCaptureSpinThread *mSpin;  // Has ownership.

    /** protectFrame protects changes of mCount and mCurrentCount */
    QMutex      mProtectFrame;

    bool        mShouldSkipUnclaimed;
    /** controls the spin thread. if locked spin thread will exit ASAP */
    QMutex      mSpinThreadRun;

public:
    bool        shouldStopSpinThread;
    uint64_t    mTimeStamp;
};
