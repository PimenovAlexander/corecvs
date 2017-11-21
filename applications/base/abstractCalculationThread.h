/**
 * \file abstractCalculationThread.h
 * \see abstractCalculationThread.cpp
 *
 * \date Aug 27, 2010
 * \author Sergey Levi
 */

#ifndef ABSTRACTCALCULATIONTHREAD_H_
#define ABSTRACTCALCULATIONTHREAD_H_

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QAtomicInt>
#include <QtCore/QMetaType>

#include <iostream>

#include "frames.h"
#include "imageCaptureInterfaceQt.h"

#include "core/filters/blocks/filterGraph.h"

/**
 * An empty stub for the output data obtained as the result of the calculation.
 * The only reason for not using \c void* is to ensure proper destruction
 *
 */

class AbstractOutputData
{
public:
    virtual ~AbstractOutputData() {}
};

class AbstractCalculationThread : public QThread
{
    Q_OBJECT

    public:
        enum CalculationState
        {
            CALCULATION_NOT_STARTED = 0, /**< Thread is just created but still not running */
            CALCULATION_AWAITING_DATA,   /**< Thread is blocked waiting for signals with frames */
            CALCULATION_ACTIVE,          /**< Thread is processing the frame */
            CALCULATION_PAUSED,          /**< Thread is paused, it will ignore incoming frames */
            CALCULATION_TERMINATED       /**< Thread has exited */
        };

        static const char *stateNames[];

    protected slots:
        /**
         *  \brief New frame data is ready to be processed.
         *
         *  Connect this slot to the newFrameReady signal emitted by the capture interface.
         *  In this slot we actually grab the frames and trigger the calculation.
         *
         *  \note Do not override this implementation unless you really know what you are doing.
         *
         *  This function processes the newFrames from the input queue.
         *  Depending on current state this function either processes of ignores
         *  input data.
         *
         *  If current state is CALCULATION_AWAITING_DATA the state is changed to
         *  CALCULATION_ACTIVE and the awaiting events queue is flushed by
         *  QCoreApplication::processEvents();
         *
         *
         *  If current state is not CALCULATION_AWAITING_DATA all input is ignored
         *
         *
         */

        virtual void newFrameReady(frame_data_t frameData);

        /** Pauses calculation. Should only be called as a asynchronous slot function */
        virtual void pauseCalculation();

        /** Resumes calculation Should only be called as a asynchronous slot function */
        virtual void resumeCalculation();

    public:

        AbstractCalculationThread();
        virtual ~AbstractCalculationThread() {}

        virtual void run();

        virtual inline void setCalculationState(CalculationState state)
        {
            mCurrentState = state;
        }

        virtual inline CalculationState getCalculationState() const
        {
            return mCurrentState;
        }

        Frames *getFramesHolder()
        {
            return &mFrames;
        }

//        QWaitCondition threadPaused;

        void setImageCaptureInterface(ImageCaptureInterfaceQt *captureInterface)
        {
            mCaptureInterface = captureInterface;
            mLastFrameTimeStamp = 0;
        }

    protected:
        /**
         * The capture interface we fetch the frames from
         **/
        ImageCaptureInterfaceQt *mCaptureInterface;

        /**
         * Holds the last two sets of frames acquired from the capture device(s)
         **/
        Frames mFrames;

        /**
         * Delay between this and previous frame
         **/
        uint64_t mInterframeDelay;

        uint64_t mLastFrameTimeStamp;

        /**
         * Current calculation state
         * This state can't be changed directly, but can be queried
         **/
        CalculationState mCurrentState;

        /**
         * This is the main calculation routine called when a new frame is received
         *
         */
        virtual AbstractOutputData *processNewData() = 0;


    signals:
        //* Notifies that a calculation iteration was completed and sends the results
        void processingFinished(AbstractOutputData *output);

        //* Notifies that the calculation state was changed
        void calculationStateChanged(AbstractCalculationThread::CalculationState state);

public:
    FiltersCollection mFiltersCollection;
};

#endif /* ABSTRACTCALCULATIONTHREAD_H_ */
