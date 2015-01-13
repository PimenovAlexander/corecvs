/**
 * \file recorderThread.cpp
 * \brief Implements a frame recording calculation thread based on BaseCalculationThread
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include "recorderThread.h"

#include <stdio.h>
#include <QtGui/QMessageBox>
#include <QMetaType>
#include <QMessageBox>

#include "g12Image.h"
#include "imageResultLayer.h"
// TEST
// #include "viFlowStatisticsDescriptor.h"

RecorderThread::RecorderThread() :
   BaseCalculationThread()
  , mRecordingStarted(false)
  , mIsRecording(false)
  , mFrameCount(0)
  , mPath("")
  , mRecorderParameters(NULL)
{
    qRegisterMetaType<RecorderThread::RecordingState>("RecorderThread::RecordingState");
    mIdleTimer = PreciseTimer::currentTime();
}

void RecorderThread::toggleRecording()
{
    if (!mIsRecording)
    {
        if (mRecorderParameters.isNull())
        {
            cout << "RecorderThread: Internal error. Recording toggled but no parameters provided." << endl;
        }

        if (mRecorderParameters->path().empty())
        {
            cout << "RecorderThread: Path is empty" << endl;
        }

        if (mRecorderParameters->fileTemplate().empty())
        {
            cout << "RecorderThread: File template is empty\n";
        }

        if (!mRecordingStarted)
        {
            mRecordingStarted = true;
            printf("Recording started.\n");
        }
        else
        {
            printf("Recording resumed.\n");
        }
        mIsRecording = true;
        emit recordingStateChanged(StateRecordingActive);
    }
    else
    {
        mIsRecording = false;
        printf("Recording paused.\n");
        emit recordingStateChanged(StateRecordingPaused);
    }
}

AbstractOutputData* RecorderThread::processNewData()
{
    Statistics stats;

    qDebug("RecorderThread::processNewData(): called");

#if 0
    stats.setTime(ViFlowStatisticsDescriptor::IDLE_TIME, mIdleTimer.usecsToNow());
#endif

    PreciseTimer start = PreciseTimer::currentTime();
    PreciseTimer startEl = PreciseTimer::currentTime();

    bool have_params = !(mRecorderParameters.isNull());
    bool two_frames = have_params && (CamerasConfigParameters::TwoCapDev == mActiveInputsNumber); // FIXME: additional params needed here

    // We are missing data, so pause calculation
    if ((!mFrames.getCurrentFrame(Frames::LEFT_FRAME) ) ||
       ((!mFrames.getCurrentFrame(Frames::RIGHT_FRAME)) && (CamerasConfigParameters::TwoCapDev == mActiveInputsNumber)))
    {
        emit errorMessage("Capture error.");
        pauseCalculation();
    }

    recalculateCache();

    G12Buffer *result[Frames::MAX_INPUTS_NUMBER] = {NULL, NULL};

    /*TODO: Logic here should be changed according to the host base change*/
    for (int id = 0; id < mActiveInputsNumber; id++)
    {
        G12Buffer *buf = mFrames.getCurrentFrame((Frames::FrameSourceId)id);

        //result[id] = mTransformationCache[id] ? mTransformationCache[id]->doDeformation(mBaseParams->interpolationType(), buf) : buf;
        result[id] = buf;

        if (mIsRecording)
        {
            char currentPath[256];
            if (two_frames)
                snprintf2buf(currentPath, mPath.toStdString().c_str(), mFrameCount, id);
            else
                snprintf2buf(currentPath, mPath.toStdString().c_str(), mFrameCount);

            if (mSaver.save(currentPath, result[id]))
            {
                resetRecording();
                emit errorMessage(QString("Error writing frame to file") + currentPath);
                emit recordingStateChanged(StateRecordingFailure);
            }
        }

    }
#if 0
    stats.setTime(ViFlowStatisticsDescriptor::CORRECTON_TIME, startEl.usecsToNow());
#endif
    if (mIsRecording) {
        mFrameCount++;
    }

    RecorderOutputData* outputData = new RecorderOutputData();

    outputData->mMainImage.addLayer(
            new ImageResultLayer(
                    mPresentationParams->output(),
                    result
            )
    );

    outputData->mMainImage.setHeight(mBaseParams->h());
    outputData->mMainImage.setWidth (mBaseParams->w());

#if 0
    stats.setTime(ViFlowStatisticsDescriptor::TOTAL_TIME, start.usecsToNow());
#endif
    mIdleTimer = PreciseTimer::currentTime();

    for (int id = 0; id < mActiveInputsNumber; id++)
    {
        if (result[id] != mFrames.getCurrentFrame((Frames::FrameSourceId)id)) {
             delete_safe(result[id]);
        }
    }

    outputData->frameCount = this->mFrameCount;
    outputData->stats = stats;

    return outputData;
}

void RecorderThread::resetRecording()
{
    mIsRecording = false;
    mRecordingStarted = false;
    mPath = "";
    mFrameCount = 0;
    emit recordingStateChanged(StateRecordingReset);
}

void RecorderThread::recorderControlParametersChanged(QSharedPointer<Recorder> recorderParameters)
{
    if (!recorderParameters)
        return;

    mRecorderParameters = recorderParameters;
    mPath = QString(recorderParameters->path().c_str()) + "/" + QString(recorderParameters->fileTemplate().c_str());
}

void RecorderThread::baseControlParametersChanged(QSharedPointer<BaseParameters> params)
{
    BaseCalculationThread::baseControlParametersChanged(params);
}

void RecorderThread::camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters)
{
    BaseCalculationThread::camerasParametersChanged(parameters);
}
