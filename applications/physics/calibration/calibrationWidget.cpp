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
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    addImage();
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
    ImageForCalibrationWidget *widget = new ImageForCalibrationWidget();
    connect(widget,SIGNAL(approved()), this , SLOT(addImage()));
    connect(widget,SIGNAL(approved()), this , SLOT(updateStartButton()));
    connect(widget,SIGNAL(closed(int)), this , SLOT(updateStartButtonAndRemoveWidget(int)));
    widget->setImage(&defaultIm);
    widget->setId(widgetCounter);
    widgetCounter++;
    widgets.push_back(widget);
    layout->addWidget(widget);
}

void CalibrationWidget::updateStartButton()
{
    //std::cout<<"here1"<<std::endl;
    if (widgets.size()>4)
    {
        ui->startCalibration->setEnabled(true);
    }
    else {
        ui->startCalibration->setEnabled(false);
    }
    //std::cout<<"here2"<<std::endl;
}

void CalibrationWidget::updateStartButtonAndRemoveWidget(int k)
{
    int size = widgets.size();
    for (int i=0;i<size;i++)
    {
        if (widgets[i]->getId() == k)
        {
            widgets.erase(widgets.begin() + i );
            size--;
        }
    }
    if (widgets.size()>4)
    {
        ui->startCalibration->setEnabled(true);
    }
    else {
        ui->startCalibration->setEnabled(false);
    }
}

void CalibrationWidget::addImage(cv::Mat)
{
    QLayout *layout = ui->imageBox->layout();
    ImageForCalibrationWidget *widget = new ImageForCalibrationWidget();
    widgets.push_back(widget);
    layout->addWidget(widget);
}


