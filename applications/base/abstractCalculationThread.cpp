/**
 * \file abstractCalculationThread.cpp
 * \brief Implements an abstract calculation thread capable of fetching frames from
          an ImageCaptureInterface instance
 *
 * \date Aug 27, 2010
 * \author Sergey Levi
 */

#include <stdlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include "global.h"

#include "abstractCalculationThread.h"
#include "log.h"

using namespace std;

const char *AbstractCalculationThread::stateNames[] =
{
    "Not started",    // CALCULATION_NOT_STARTED = 0, /**< Thread is just created but still not running */
    "Awaiting data",  // CALCULATION_AWAITING_DATA,   /**< Thread is blocked waiting for signals with frames */
    "Active",         // CALCULATION_ACTIVE,          /**< Thread is processing the frame */
    "Paused",         // CALCULATION_PAUSED,          /**< Thread is paused, it will ignore incoming frames */
    "Terminated"      // CALCULATION_TERMINATED       /**< Thread has exited */
};

STATIC_ASSERT (CORE_COUNT_OF(AbstractCalculationThread::stateNames) ==
               AbstractCalculationThread::CALCULATION_TERMINATED + 1, wrong_state_number);


AbstractCalculationThread::AbstractCalculationThread() :
    mCaptureInterface(NULL)
  , mInterframeDelay(0)
  , mLastFrameTimeStamp(0)
  , mCurrentState(CALCULATION_NOT_STARTED)
{
    qRegisterMetaType<AbstractCalculationThread::CalculationState>("AbstractCalculationThread::CalculationState");

    // TODO: this is a workaround to fix the problem of AbstractCalculationThread slots
    //       being processed in a wrong thread context.
    //       AbstractCalculationThread should not inherit from QThread but rather be
    //       created in the context of a new thread.
    moveToThread(this);
}


void AbstractCalculationThread::run()
{
//    stateLock.lock();
    mCurrentState = CALCULATION_AWAITING_DATA;
//    stateLock.unlock();

    cout << "AbstractCalculationThread::run(): starting event loop.\n";
    exec();
    cout << "AbstractCalculationThread::run(): event loop terminated.\n";

//    stateLock.lock();
    mCurrentState = CALCULATION_TERMINATED;
//    stateLock.unlock();
}

void AbstractCalculationThread::newFrameReady(frame_data_t frameData)
{
//    cout << "AbstractCalculationThread::newFrameReady called..." << endl;

    //L_INFO_P("new frame notification");

    if (!mCaptureInterface)
    {
        cerr << "Image capture interface was not initialized\n";
        return;
    }

    if (mCurrentState != CALCULATION_AWAITING_DATA) {
        if (mCurrentState == CALCULATION_ACTIVE) {
            DOTRACE(("AbstractCalculationThread::dropped frame\n"));
        }
        return;
    }

    if (frameData <= mLastFrameTimeStamp) /*An overloaded operator is used*/
    {
        cout << "Frame was received second time. "
                "Previous TS: (" << mLastFrameTimeStamp.streamId << ") " << mLastFrameTimeStamp.timestamp <<
                " Current TS: (" << frameData.streamId           << ") "<< frameData.timestamp << "\n";
        return;
    }

    mCurrentState = CALCULATION_ACTIVE;
    //emit calculationStateChanged(getCalculationState());

    /**
     * Here is the trick, this function can call newFrameReady recursively,
     * but it will exit with dropped frame.
     * No other function can change currentState, only next state is altered.
     *
     **/
    QCoreApplication::processEvents();

    /* On exit from processEvents() we have flushed input queue of the thread */
    /* State can be changed already while in processEvents */
    if (mCurrentState == CALCULATION_ACTIVE)
    {
        uint64_t oldStamp   = mFrames.timestamp();
        mFrames.fetchNewFrames(mCaptureInterface);

        mInterframeDelay    = mFrames.timestamp() - oldStamp;
        mLastFrameTimeStamp = mFrames.frameHerald();
/*
        cout << "AbstractCalculationThread::requesting frame..." << endl;
        if (interframeDelay == 0) {
            printf("Strange - two frames with same timestamp\n");
        }
*/
        bool ok = false;

        for (int id = 0; id <  Frames::MAX_INPUTS_NUMBER; id++) {
            ok |= (mFrames.getCurrentFrame((Frames::FrameSourceId)id) != NULL);
        }

        if (ok) /* At least one input is active */
        {
            AbstractOutputData *res = processNewData();
            if (res != NULL)
            {
                /* TODO: Possibly needs redesign. If signal is not processed there will
                be a memory leak. Use Smart pointers here */
                emit processingFinished(res);
            }
        } else {
            SYNC_PRINT(("AbstractCalculationThread::newFrameReady(): No good frames. All frames are NULL\n"));
        }
    }

    if (mCurrentState == CALCULATION_PAUSED) {
        emit calculationStateChanged(mCurrentState);
//        threadPaused.wakeAll();
    } else {
        mCurrentState = CALCULATION_AWAITING_DATA;
    }
}

void AbstractCalculationThread::pauseCalculation()
{
    if ((mCurrentState != CALCULATION_ACTIVE) && (mCurrentState != CALCULATION_AWAITING_DATA))
    {
        cout << "There is no calculation in progress - can't pause.\n";
        return;
    }

    if (mCurrentState == CALCULATION_AWAITING_DATA)
    {
        mCurrentState = CALCULATION_PAUSED;
        emit calculationStateChanged(mCurrentState);
//        threadPaused.wakeAll();
    }
    mCurrentState = CALCULATION_PAUSED;
}

void AbstractCalculationThread::resumeCalculation()
{
    if (mCurrentState == CALCULATION_NOT_STARTED)
    {
        cout << "Use start() for the first time.\n";
        return;
    }

    if (mCurrentState == CALCULATION_PAUSED)
    {
        mCurrentState = CALCULATION_AWAITING_DATA;
        emit calculationStateChanged(mCurrentState);
    }
}
