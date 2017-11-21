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

#include "core/utils/global.h"

#include "baseCalculationThread.h"
#include "imageCaptureInterface.h"
#include "core/fileformats/ppmLoader.h"
#include "core/utils/preciseTimer.h"
#include "generatedParameters/scannerParameters.h"
#include "core/stats/calculationStats.h"


class ScannerOutputData : public BaseOutputData
{
public:
    Statistics stats;
    unsigned frameCount;

    Mesh3D outputMesh;

    vector<double> cut;
    vector<double> cutConvolution;

    G8Buffer *brightness = NULL;
    RGB24Buffer *corners   = NULL;
    RGB24Buffer *convolution = NULL;
    G8Buffer *channel = NULL;

    virtual ~ScannerOutputData() override
    {
        delete_safe(brightness);
        delete_safe(convolution);
        delete_safe(channel);
        delete_safe(corners);
    }

};

class ScannerThread : public BaseCalculationThread
{
    Q_OBJECT

public:


    enum ScanningState
    {
        IDLE,
        SCANNING,
        MOVING,
        HOMEING,
        PAUSED
    };


    int scanCount;
    static const int MAX_COUNT=500;



    ScannerThread();

public slots:
    void toggleScanning();

    void scannerControlParametersChanged(QSharedPointer<ScannerParameters> params);
    void baseControlParametersChanged(QSharedPointer<BaseParameters> params);
    void camerasParametersChanged(QSharedPointer<CamerasConfigParameters> parameters);

    void homeingWaitingFinished();
    void scanningWaitingFinished();

signals:
    void scanningStateChanged(ScannerThread::ScanningState state);

protected:
    virtual AbstractOutputData *processNewData();

private:
    bool mScanningStarted;
    bool mIsScanning;
    bool timeToSave;

    PreciseTimer mIdleTimer;

    /* Might be misleading, but PPMLoader handles saving as well */
    PPMLoader mSaver;

    uint32_t mFrameCount;
    QString mPath;
    QSharedPointer<ScannerParameters> mScannerParameters;

};

#endif /* SCANNERTHREAD_H_ */
