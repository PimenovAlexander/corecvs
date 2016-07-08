/**
 * \file scannerThread.cpp
 * \brief Implements a frame recording calculation thread based on BaseCalculationThread
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include "scannerThread.h"

#include <stdio.h>
#include <QMetaType>
#include <QMessageBox>

#include "g12Image.h"
#include "imageResultLayer.h"
// TEST
// #include "viFlowStatisticsDescriptor.h"

ScannerThread::ScannerThread() :
   BaseCalculationThread()
  , mRecordingStarted(false)
  , mIsRecording(false)
  , mFrameCount(0)
  , mPath("")
  , mScannerParameters(NULL)
{
    qRegisterMetaType<ScannerThread::RecordingState>("ScannerThread::RecordingState");
    mIdleTimer = PreciseTimer::currentTime();
}

void ScannerThread::toggleRecording()
{
    if (!mIsRecording)
    {
        if (mScannerParameters.isNull())
        {
            cout << "ScannerThread: Internal error. Recording toggled but no parameters provided." << endl;
        }

        if (mScannerParameters->path().empty())
        {
            cout << "ScannerThread: Path is empty" << endl;
        }

        if (mScannerParameters->fileTemplate().empty())
        {
            cout << "ScannerThread: File template is empty\n";
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

AbstractOutputData* ScannerThread::processNewData()
{
    Statistics stats;

//    qDebug("ScannerThread::processNewData(): called");

#if 0
    stats.setTime(ViFlowStatisticsDescriptor::IDLE_TIME, mIdleTimer.usecsToNow());
#endif

    PreciseTimer start = PreciseTimer::currentTime();
//    PreciseTimer startEl = PreciseTimer::currentTime();

    bool have_params = !(mScannerParameters.isNull());
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
        G12Buffer   *buf    = mFrames.getCurrentFrame   ((Frames::FrameSourceId)id);
        RGB24Buffer *bufrgb = mFrames.getCurrentRgbFrame((Frames::FrameSourceId)id);
        if (bufrgb != NULL) {
            buf = bufrgb->toG12Buffer();
        }


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

    ScannerOutputData* outputData = new ScannerOutputData();

    outputData->mMainImage.addLayer(
            new ImageResultLayer(
                    mPresentationParams->output(),
                    result
            )
    );

    outputData->mMainImage.setHeight(mBaseParams->h());
    outputData->mMainImage.setWidth (mBaseParams->w());

#if 1
    stats.setTime("Total time", start.usecsToNow());
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

void ScannerThread::resetRecording()
{
    mIsRecording = false;
    mRecordingStarted = false;
    mPath = "";
    mFrameCount = 0;
    emit recordingStateChanged(StateRecordingReset);
}

void ScannerThread::scannerControlParametersChanged(QSharedPointer<Scanner> scannerParameters)
{
    if (!scannerParameters)
        return;

    mScannerParameters = scannerParameters;
    mPath = QString(scannerParameters->path().c_str()) + "/" + QString(scannerParameters->fileTemplate().c_str());
}

void ScannerThread::baseControlParametersChanged(QSharedPointer<BaseParameters> params)
{
    BaseCalculationThread::baseControlParametersChanged(params);
}

void ScannerThread::camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters)
{
    BaseCalculationThread::camerasParametersChanged(parameters);
}
