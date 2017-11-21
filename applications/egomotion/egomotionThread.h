/**
 * \file egomotionThread.h
 * \see egomotionThread.cpp
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#ifndef EGOMOTION_THREAD_H_
#define EGOMOTION_THREAD_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <QString>

#include "core/utils/global.h"

#include "baseCalculationThread.h"
#include "imageCaptureInterface.h"
#include "core/fileformats/ppmLoader.h"
#include "core/utils/preciseTimer.h"
#include "generated/egomotionParameters.h"
#include "core/stats/calculationStats.h"

class EgomotionOutputData : public BaseOutputData
{
public:
    Statistics stats;
    unsigned frameCount;
};

class EgomotionThread : public BaseCalculationThread
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

    EgomotionThread();

public slots:

    void egomotionControlParametersChanged(QSharedPointer<EgomotionParameters> params);
    void baseControlParametersChanged(QSharedPointer<BaseParameters> params);
    void camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters);


signals:
    void recordingStateChanged(EgomotionThread::RecordingState state);
    void errorMessage(QString string);

protected:
    virtual AbstractOutputData *processNewData();

private:
    PreciseTimer mIdleTimer;

    /* Might be misleading, but PPMLoader handles saving as well */
    PPMLoader mSaver;


    uint32_t mFrameCount;
    QSharedPointer<EgomotionParameters> mRecorderParameters;
};

#endif /* EGOMOTION_THREAD_H_ */
