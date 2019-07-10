#ifndef IMAGEFORCALIBRATIONWIDGET_H
#define IMAGEFORCALIBRATIONWIDGET_H

#include <QWidget>

namespace Ui {
class ImageForCalibrationWidget;
}

class ImageForCalibrationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageForCalibrationWidget(QWidget *parent = nullptr);
    ~ImageForCalibrationWidget();

private:
    Ui::ImageForCalibrationWidget *ui;
};

#endif // IMAGEFORCALIBRATIONWIDGET_H
