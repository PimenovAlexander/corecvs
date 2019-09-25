/**
 * \file copterThread.h
 * \see copterThread.cpp
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#ifndef COPTERTHREAD_H_
#define COPTERTHREAD_H_

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <QString>

#include "core/utils/global.h"

#include "baseCalculationThread.h"
#include "core/framesources/imageCaptureInterface.h"
#include "core/fileformats/ppmLoader.h"
#include "core/utils/preciseTimer.h"
#include "generatedParameters/copter.h"
#include "core/stats/calculationStats.h"

class CopterOutputData : public BaseOutputData
{
public:
    Affine3DQ position;

    Statistics stats;
    unsigned frameCount;
};

class CopterThread : public BaseCalculationThread
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

    CopterThread();

public slots:
    void copterControlParametersChanged(QSharedPointer<Copter> params);
    void baseControlParametersChanged(QSharedPointer<BaseParameters> params);
    void camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters);


signals:
    void errorMessage(QString string);

protected:
    virtual AbstractOutputData *processNewData();

private:
    PreciseTimer mIdleTimer;

    /* Might be misleading, but PPMLoader handles saving as well */
    PPMLoader mSaver;

    uint32_t mFrameCount;
    QSharedPointer<Copter> mCopterParameters;
};

#endif /* COPTERTHREAD_H_ */
