#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>
#include "opencv2/core.hpp"
#include "imageForCalibrationWidget.h"
#include <mutex>

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
    void addImage(cv::Mat);

    void addImage();
    void updateStartButton();
    void updateStartButtonAndRemoveWidget(int k);
    void startCalibration();
    void startRecording();
    void updateVideo();
    void on_videoBox_currentIndexChanged(int index);

    void updateImage();
    void stopShowing();
    void saveMatrix();
private:
    int cameraNumber=-1;
    Ui::CalibrationWidget *ui;
    std::vector<ImageForCalibrationWidget *> widgets;
    int widgetCounter=0;
    bool caputing = false;
    bool threadRunning = false;
    std::mutex caputingMutex;
    void setCapute(bool b);
    bool getCapute();
    std::mutex vectorMutex;

    cv::Mat globalIntrinsic;
    cv::Mat globalDistCoeffs;

    bool getShowingBool();
    bool showingBool = false;
    void setShowingBool(bool b);
    std::mutex showingMutex;

    void stopRecording();
    void calibrate();
};


#endif // CALIBRATIONWIDGET_H
