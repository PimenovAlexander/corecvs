#ifndef OPTICALFLOWANALYSIS_H
#define OPTICALFLOWANALYSIS_H

#include <QObject>
#include <imageCaptureInterfaceQt.h>

#include "KLTFlow.h"


class OpticalFlowAnalysis : public QObject
{
    Q_OBJECT
public:
    Processor6D *opticalFlowProcessor = NULL;
    explicit OpticalFlowAnalysis(QObject *parent = 0);

signals:

public slots:
    void analyzeFlow();
};

#endif // OPTICALFLOWANALYSIS_H
