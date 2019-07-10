#ifndef CALIBRATIONWIDGET_H
#define CALIBRATIONWIDGET_H

#include <QWidget>
#include "opencv2/core.hpp"
#include "imageForCalibrationWidget.h"

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
private:
    Ui::CalibrationWidget *ui;
     std::vector<ImageForCalibrationWidget *> widgets;
};

#endif // CALIBRATIONWIDGET_H
