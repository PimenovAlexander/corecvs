/**
 * \file scannerThread.h
 * \see scannerThread.cpp
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#ifndef SCANNERTHREAD_H_
#define SCANNERTHREAD_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <QString>

#include "global.h"

#include "baseCalculationThread.h"
#include "imageCaptureInterface.h"
#include "ppmLoader.h"
#include "preciseTimer.h"
#include "generatedParameters/scannerParameters.h"
#include "calculationStats.h"

class ScannerOutputData : public BaseOutputData
{
public:
    Statistics stats;
    unsigned frameCount;

    Mesh3D outputMesh;

    vector<double> cut;
};

class ScannerThread : public BaseCalculationThread
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

    ScannerThread();

public slots:
    void toggleRecording();
    void resetRecording();

    void scannerControlParametersChanged(QSharedPointer<ScannerParameters> params);
    void baseControlParametersChanged(QSharedPointer<BaseParameters> params);
    void camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters);


signals:
    void recordingStateChanged(ScannerThread::RecordingState state);
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
    QSharedPointer<ScannerParameters> mScannerParameters;
};

#endif /* SCANNERTHREAD_H_ */
