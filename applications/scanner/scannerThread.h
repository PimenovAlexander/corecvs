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
    vector<double> cutConvolution;


    RGB24Buffer *convolution = NULL;

    virtual ~ScannerOutputData() override
    {
        delete_safe(convolution);
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


signals:
    void scanningStateChanged(ScannerThread::ScanningState state);

protected:
    virtual AbstractOutputData *processNewData();

private:
    bool mScanningStarted;
    bool mIsScanning;
    PreciseTimer mIdleTimer;

    /* Might be misleading, but PPMLoader handles saving as well */
    PPMLoader mSaver;

    uint32_t mFrameCount;
    QString mPath;
    QSharedPointer<ScannerParameters> mScannerParameters;

};

#endif /* SCANNERTHREAD_H_ */
