/**
 * \file recorderThread.h
 * \see recorderThread.cpp
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#ifndef RECORDERTHREAD_H_
#define RECORDERTHREAD_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <QString>

#include "global.h"

#include "baseCalculationThread.h"
#include "imageCaptureInterface.h"
#include "ppmLoader.h"
#include "preciseTimer.h"
#include "generatedParameters/recorder.h"
#include "calculationStats.h"

class RecorderOutputData : public BaseOutputData
{
public:
    Statistics stats;
    unsigned frameCount;
};

class RecorderThread : public BaseCalculationThread
{
    Q_OBJECT

public:
    enum RecordingState
    {
        StateRecordingActive = 0,
        StateRecordingPaused,
        StateRecordingReset,
        StateRecordingFailure
    };

    RecorderThread();

public slots:
    void toggleRecording();
    void resetRecording();

    void recorderControlParametersChanged(QSharedPointer<Recorder> params);
    void baseControlParametersChanged(QSharedPointer<BaseParameters> params);
    void camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters);


signals:
    void recordingStateChanged(RecorderThread::RecordingState state);
    void errorMessage(QString string);

protected:
    virtual AbstractOutputData *processNewData();

private:
    bool mRecordingStarted;
    bool mIsRecording;
    PreciseTimer mIdleTimer;

    /* Might be misleading, but PPMLoader handles saving as well */
    PPMLoader mSaver;

    uint32_t mFrameCount;
    QString mPath;
    QSharedPointer<Recorder> mRecorderParameters;
};

#endif /* RECORDERTHREAD_H_ */
