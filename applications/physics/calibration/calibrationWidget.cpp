#include "calibrationWidget.h"
#include "ui_calibrationWidget.h"
#include "opencv2/core.hpp"
#include "imageForCalibrationWidget.h"
#include "iostream"
#include <opencv2/opencv.hpp>



CalibrationWidget::CalibrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibrationWidget)
{
    ui->setupUi(this);

}

CalibrationWidget::~CalibrationWidget()
{
    delete ui;
}

void CalibrationWidget::addImage()
{
    QLayout *layout = ui->imageBox->layout();

    cv::Mat defaultIm;
    defaultIm = cv::imread("test3.jpg");
    cv::imshow("adadadadad",defaultIm);
    ImageForCalibrationWidget *widget = new ImageForCalibrationWidget();
    widget->setImage(&defaultIm);
    widgets.push_back(widget);
    layout->addWidget(widget);
}

void CalibrationWidget::addImage(cv::Mat)
{
    QLayout *layout = ui->imageBox->layout();

    ImageForCalibrationWidget *widget = new ImageForCalibrationWidget();
    widgets.push_back(widget);
    layout->addWidget(widget);
}


