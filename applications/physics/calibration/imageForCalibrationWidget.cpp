#include "imageForCalibrationWidget.h"
#include "ui_imageForCalibrationWidget.h"
#include "opencv2/core.hpp"
#include "opencvTransformations.h"
#include <opencv2/opencv.hpp>
#include <QGraphicsScene>


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
    inputMat->copyTo(image);
    qimage = opencvTransformations::mat2RealQImage(*inputMat).copy();
    imageSet = true;
    sc = new QGraphicsScene(this);
    sc->addPixmap(QPixmap::fromImage(qimage));
    //sc->setSceneRect()
    ui->graphicsView->setScene(sc);
    ui->graphicsView->fitInView( QRect(11,11,531,271),Qt::KeepAspectRatio);
    //ui->imageLabel->setPixmap(QPixmap::fromImage(qimage));
}

void ImageForCalibrationWidget::showImage()
{
    if (imageSet)
    {
        cv::imshow("image",image);
    }
}

void ImageForCalibrationWidget::approve()
{
    imageApproved = true;
    ui->addButon->setText("remove");
    emit (approved());
}

void ImageForCalibrationWidget::closeWid()
{
    if (!imageApproved)
    {
        approve();
    }
    else {
        emit (closed(id));
        this->~ImageForCalibrationWidget();
    }
}

void ImageForCalibrationWidget::setId(int i)
{
    if (id == -1)
    {
        id = i;
    }
}

int ImageForCalibrationWidget::getId()
{
    return id;
}
