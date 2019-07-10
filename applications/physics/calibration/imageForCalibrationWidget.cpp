#include "imageForCalibrationWidget.h"
#include "ui_imageForCalibrationWidget.h"
#include "opencv2/core.hpp"
#include "opencvTransformations.h"

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

void ImageForCalibrationWidget::setImage(cv::Mat *inputMat)
{
    QImage image = opencvTransformations::mat2RealQImage(*inputMat);
    ui->imageLabel->setPixmap(QPixmap::fromImage(image));
}
