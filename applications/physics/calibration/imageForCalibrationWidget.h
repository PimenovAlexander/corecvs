#ifndef IMAGEFORCALIBRATIONWIDGET_H
#define IMAGEFORCALIBRATIONWIDGET_H

#include <QWidget>
#include <opencv2/core.hpp>

namespace Ui {
class ImageForCalibrationWidget;
}

class ImageForCalibrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageForCalibrationWidget(QWidget *parent = nullptr);
    ~ImageForCalibrationWidget();

    void setImage(cv::Mat *);
private:
    Ui::ImageForCalibrationWidget *ui;
};

#endif // IMAGEFORCALIBRATIONWIDGET_H
