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

#include "global.h"

#include "baseCalculationThread.h"
#include "imageCaptureInterface.h"
#include "ppmLoader.h"
#include "preciseTimer.h"
#include "generated/egomotionParameters.h"
#include "calculationStats.h"
#include "eulerAngles.h"

class EgomotionOutputData : public BaseOutputData
{
public:
    Statistics stats;
    unsigned frameCount;

    RGB24Buffer *debugOutput = NULL;
    Matrix33 rot[4];
    Vector3dd trans[4];

    CameraAnglesLegacy result;


    virtual ~EgomotionOutputData()
    {
        delete_safe(debugOutput);
    }
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
    QSharedPointer<EgomotionParameters> mEgomotionParameters;

    /*Egomotion compultation state */
    G12Buffer* oldFrame = NULL;
    bool mark = false;

};

#endif /* EGOMOTION_THREAD_H_ */
