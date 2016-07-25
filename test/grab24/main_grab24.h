#ifndef MAIN_GRAB24_H_
#define MAIN_GRAB24_H_

#include <QObject>
#include "imageCaptureInterface.h"
#include "advancedImageWidget.h"
#include "v4l2/V4L2Capture.h"

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
