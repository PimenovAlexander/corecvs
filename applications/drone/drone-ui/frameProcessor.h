#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H
#include <QThread>
#include <patternDetectorParametersWidget.h>
#include <core/framesources/imageCaptureInterface.h>
#include <core/cameracalibration/cameraModel.h>
#include <patterndetection/patternDetector.h>


class ImageCaptureInterfaceQt;
class PhysicsMainWindow;


/**
 * Ok this a draft. In general most probably we should not depend on QThread.
 **/
class FrameProcessor : public QThread
{
    Q_OBJECT
public:

    PhysicsMainWindow *target = NULL;
    ImageCaptureInterfaceQt *input = NULL;
    FrameProcessor(QObject *parent = NULL);

   /* Main setup */
    PatternDetector *detector = NULL;
    CameraModel mCameraModel;
    GeneralPatternDetectorParameters mPatternToPose;

public slots:
    /** NB: This is a place to process video **/
    void processFrame(ImageCaptureInterface::FrameMetadata frameData);


    void setPatternDetectorParameters(GeneralPatternDetectorParameters params);
    void setCameraModel              (CameraModel params);

    virtual void run()
    {
        exec();
    }
};



#endif // FRAMEPROCESSOR_H
