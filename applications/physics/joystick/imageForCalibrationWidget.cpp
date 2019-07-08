#include "imageForCalibrationWidget.h"
#include "ui_imageForCalibrationWidget.h"

ImageForCalibrationWidget::ImageForCalibrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageForCalibrationWidget)
{
    ui->setupUi(this);
}

ImageForCalibrationWidget::~ImageForCalibrationWidget()
{
    delete ui;
}
