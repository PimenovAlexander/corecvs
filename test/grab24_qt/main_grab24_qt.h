#ifndef MAIN_GRAB24_H_
#define MAIN_GRAB24_H_

#include "core/framesources/imageCaptureInterface.h"
#include <QObject>

class V4L2CaptureInterface;
class AdvancedImageWidget;

class FrameProcessor : public QObject
{
    Q_OBJECT
public:

    V4L2CaptureInterface *input = NULL;
    AdvancedImageWidget *widget = NULL;

    FrameProcessor(QObject *parent = 0);

public slots:
    void processFrame(frame_data_t frameData);

};

#endif
