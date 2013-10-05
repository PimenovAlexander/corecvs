#pragma once

#include <QtCore/QThread>

#include "imageCaptureInterface.h"

class AbstractFileCapture;

class AbstractFileCaptureSpinThread : public QThread
{
public:
    AbstractFileCaptureSpinThread(AbstractFileCapture *pInterface, int delay, ImageCaptureInterface::FramePair &framePair);
    virtual ~AbstractFileCaptureSpinThread() {}

    virtual void run();
    void         togglePause();
    void         nextFrame();

    virtual bool grabFramePair() = 0;

protected:
    ImageCaptureInterface::FramePair &mFramePair;
    AbstractFileCapture *mInterface;  // Doesn't have ownership.

    void pause();

private:
    bool mPaused;
    bool mNextFrameNeeded;
    int  mDelay;
};
