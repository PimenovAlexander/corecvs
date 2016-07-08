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
#if 0
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
#endif

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

    RGB24Buffer *bufrgb = mFrames.getCurrentRgbFrame(Frames::RIGHT_FRAME);

    stats.startInterval();
    RGB24Buffer red(bufrgb);

    for (int i = 0; i < red.h; i++)
        for (int j = 0; j < red.w; j++)
        {
            RGBColor &pixel = red.element(i,j);
            if (pixel.r() <= mScannerParameters->redThreshold())
                pixel = RGBColor::Black();
        }
    stats.endInterval("Removing red");

    ScannerOutputData* outputData = new ScannerOutputData();

    outputData->mMainImage.addLayer(
            new ImageResultLayer(                   
                    &red
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

void ScannerThread::scannerControlParametersChanged(QSharedPointer<ScannerParameters> scannerParameters)
{
    if (!scannerParameters)
        return;

    mScannerParameters = scannerParameters;
   // mPath = QString(scannerParameters->path().c_str()) + "/" + QString(scannerParameters->fileTemplate().c_str());
}

void ScannerThread::baseControlParametersChanged(QSharedPointer<BaseParameters> params)
{
    BaseCalculationThread::baseControlParametersChanged(params);
}

void ScannerThread::camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters)
{
    BaseCalculationThread::camerasParametersChanged(parameters);
}
