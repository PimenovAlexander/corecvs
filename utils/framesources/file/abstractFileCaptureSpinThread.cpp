#include "abstractFileCaptureSpinThread.h"
#include "abstractFileCapture.h"

AbstractFileCaptureSpinThread::AbstractFileCaptureSpinThread(
    AbstractFileCapture *pInterface
    , int delay
    , ImageCaptureInterface::FramePair &framePair
    )
    : mFramePair(framePair)
    , mInterface(pInterface)
    , mPaused(false)
    , mNextFrameNeeded(false)
    , mDelay(delay)
{
}

void AbstractFileCaptureSpinThread::run()
{
    if (!grabFramePair())  // We must get a frame before querying for properties
    {
        printf("No frames.\n");
        return;
    }

    frame_data_t frameData;
    frameData.timestamp = mInterface->mTimeStamp;
    mInterface->notifyAboutNewFrame(frameData);

    while (mInterface->spinThreadRunMutex().tryLock())
    {
        if (mDelay != 0) {
            msleep(mDelay);
        }

        if (!mPaused || mNextFrameNeeded)
        {
            mNextFrameNeeded = false;

            mInterface->protectFrameMutex().lock();
                if (!grabFramePair()) {
                    printf("No more frames.\n");
                }
                else {
                    frame_data_t frameData;
                    frameData.timestamp = mInterface->mTimeStamp;
                    mInterface->notifyAboutNewFrame(frameData);
                }
            mInterface->protectFrameMutex().unlock();
        }


        mInterface->spinThreadRunMutex().unlock();

        if (mInterface->shouldStopSpinThread) {
            break;
        }
    }
}

void AbstractFileCaptureSpinThread::togglePause()
{
    mPaused = !mPaused;
}

void AbstractFileCaptureSpinThread::nextFrame()
{
    mNextFrameNeeded = true;
}

void AbstractFileCaptureSpinThread::pause()
{
    mPaused = true;
    mInterface->notifyAboutStreamPaused();
}
