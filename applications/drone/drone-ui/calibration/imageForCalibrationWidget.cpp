#include "imageForCalibrationWidget.h"
#include "ui_imageForCalibrationWidget.h"
#include "opencv2/core.hpp"
#include "opencvTransformations.h"
#include <opencv2/opencv.hpp>
#include <QGraphicsScene>
#include <g12Image.h>


ImageForCalibrationWidget::ImageForCalibrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageForCalibrationWidget)
{
    ui->setupUi(this);
}

ImageForCalibrationWidget::~ImageForCalibrationWidget()
{
    delete_safe(rgb24Image);
    delete ui;
}

void ImageForCalibrationWidget::setImage(corecvs::RGB24Buffer *input)
{
    delete_safe(rgb24Image);
    rgb24Image = input;
    qImage = new RGB24Image(input);
    updateThumbnail();
    unlockButtons();
}

corecvs::RGB24Buffer *ImageForCalibrationWidget::getImage()
{
    return rgb24Image;
}

void ImageForCalibrationWidget::updateThumbnail()
{
    QImage scaled = qImage->scaled(qImage->width(), 200, Qt::KeepAspectRatio);
    SYNC_PRINT(("Target widget size : %d x %d\n", ui->widget->width(), ui->widget->height()));
    ui->widget->setImage(new QImage(scaled));
}

void ImageForCalibrationWidget::setDefaultImage()
{   
    updateThumbnail();
}


void ImageForCalibrationWidget::lockButtons()
{
    ui->addButon->setEnabled(false);
    ui->showButon->setEnabled(false);
}

void ImageForCalibrationWidget::unlockButtons()
{
    ui->addButon->setEnabled(true);
    ui->showButon->setEnabled(true);
}


