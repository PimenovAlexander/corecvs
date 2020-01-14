#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>
#include "opencv2/core.hpp"
#include "imageForCalibrationWidget.h"
#include <capSettingsDialog.h>
#include <g12Image.h>
#include <imageCaptureInterfaceQt.h>
#include <inputSelectorWidget.h>
#include <mutex>
#include <cameracalibration/cameraModel.h>

namespace Ui {
class CalibrationWidget;
}

class CalibrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalibrationWidget(QWidget *parent = nullptr);
    ~CalibrationWidget();

private slots:
    void addImage(corecvs::RGB24Buffer *image);
    void updateStartButton();
    void startCalibration();
    void undistort();


    void updateImage();
    void stopShowing();
    void saveMatrix();

    /* Camera control */
public:
    InputSelectorWidget mInputSelector;
    CapSettingsDialog mCameraParametersWidget;

    ImageCaptureInterfaceQt *mInterface = NULL;
    RGB24Buffer *currentImage = NULL;
public slots:
    void showInputSettings();
    void showCameraSettings();

public slots:
    void startRecording();
    void pauseRecording();
    void stopRecording();

    void newFrameRequset();

public:
    /* */
    vector<SaveableWidget *> mSaveableWidgets;

    /** Maintain the list of images **/
public slots:
    void captureImage();

    /** Load and Save **/
public slots:
    void load(void);
    void save(void);

private:
    int cameraNumber=-1;

    std::vector<ImageForCalibrationWidget *> widgets;

    cv::Mat mIntrinsic;
    cv::Mat mDistCoeffs;
    corecvs::CameraModel mCameraModel;

    bool getShowingBool();
    bool showingBool = false;
    void setShowingBool(bool b);
    std::mutex showingMutex;

    void calibrate();
    Ui::CalibrationWidget *ui;
};


#endif // CALIBRATIONWIDGET_H
