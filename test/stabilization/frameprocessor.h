#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include <QObject>
#include <imageCaptureInterfaceQt.h>
#include "uis/advancedImageWidget.h"
#include "KLTFlow.h"

class FrameProcessor : public QObject
{
    Q_OBJECT
public:
    ImageCaptureInterfaceQt *input = NULL;
    AdvancedImageWidget *widget = NULL;
    Processor6D *optucalFlowProcessor = NULL;

    FrameProcessor(QObject *parent = 0);

signals:
    void flowCalculated();

public slots:
    void processFrame();

};

#endif // FRAMEPROCESSOR_H
