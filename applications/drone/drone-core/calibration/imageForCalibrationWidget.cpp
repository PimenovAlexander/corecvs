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
    setDefaultImage();
}

ImageForCalibrationWidget::~ImageForCalibrationWidget()
{
    delete ui;
}

void ImageForCalibrationWidget::setImage(cv::Mat *inputMat)
{
    inputMat->copyTo(image);
    qimage = opencvTransformations::mat2RealQImage(image).copy();
    imageSet = true;
    //sc = new QGraphicsScene(this);
    //sc->addPixmap(QPixmap::fromImage(qimage));
    //ui->graphicsView->setScene(sc);
    //ui->graphicsView->fitInView( QRect(11,11,431,221),Qt::KeepAspectRatio);
    //updateMicroImage();
    unlockButtons();
}

cv::Mat ImageForCalibrationWidget::getImage()
{
    return image;
}

void ImageForCalibrationWidget::updateMicroImage()
{
    sc = new QGraphicsScene(this);
    sc->addPixmap(QPixmap::fromImage(qimage));
    ui->graphicsView->setScene(sc);
    ui->graphicsView->fitInView( QRect(0,0,640,480),Qt::KeepAspectRatio);
}

void ImageForCalibrationWidget::setDefaultImage()
{
    cv::Mat inputMat = cv::imread("test1.jpg");
    inputMat.copyTo(image);
    qimage = opencvTransformations::mat2RealQImage(image).copy();
    //sc = new QGraphicsScene(this);
    //sc->addPixmap(QPixmap::fromImage(qimage));
    //ui->graphicsView->setScene(sc);
    //ui->graphicsView->fitInView( QRect(11,11,211,111),Qt::KeepAspectRatio);
    updateMicroImage();
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
