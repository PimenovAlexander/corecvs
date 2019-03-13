#ifndef MAIN_GRAB24_H_
#define MAIN_GRAB24_H_

#include <QObject>
#include <imageCaptureInterfaceQt.h>

class AdvancedImageWidget;

class FrameProcessor : public QObject
{
    Q_OBJECT
public:

    ImageCaptureInterfaceQt *input = NULL;
    AdvancedImageWidget *widget = NULL;

    FrameProcessor(QObject *parent = 0);

public slots:
    void processFrame(ImageCaptureInterface::FrameMetadata frameData);

};

#endif
