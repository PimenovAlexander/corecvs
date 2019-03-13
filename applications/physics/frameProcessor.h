#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H
#include <QThread>
#include <core/framesources/imageCaptureInterface.h>


class ImageCaptureInterfaceQt;
class PhysicsMainWidget;


/**
 * Ok this a draft. In general most probably we should not depend on QThread.
 **/
class FrameProcessor : public QThread
{
    Q_OBJECT
public:

    PhysicsMainWidget *target = NULL;
    ImageCaptureInterfaceQt *input = NULL;
    FrameProcessor(QObject *parent = 0) : QThread(parent)
    {}

   /* Main setup */



public slots:
    /** NB: This is a place to process video **/
    void processFrame(ImageCaptureInterface::FrameMetadata frameData);


    virtual void run()
    {
        exec();
    }
};



#endif // FRAMEPROCESSOR_H
