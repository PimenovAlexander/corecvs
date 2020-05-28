#pragma once

#include <thread>
#include "framesources/imageCaptureInterface.h"

class AbstractFileCapture;

class AbstractFileCaptureSpinThread /*: public QThread*/
{
public:
    AbstractFileCaptureSpinThread(AbstractFileCapture *pInterface, int delay, ImageCaptureInterface::FramePair &framePair);
    virtual ~AbstractFileCaptureSpinThread();

    /** This is because is legacy design. Kind of stupid. If you have courage - rework this **/
    static void launchThread (AbstractFileCaptureSpinThread *object);

    void         togglePause();
    void         nextFrame();

    virtual bool grabFramePair() = 0;
    void run ();


    void start();
    void pause();
    void join ();

protected:
    ImageCaptureInterface::FramePair &mFramePair;
    AbstractFileCapture *mInterface;  // Doesn't have ownership.

    std::thread  *mThread = NULL;
private:
    bool mPaused;
    bool mNextFrameNeeded;
    int  mDelay;
};

