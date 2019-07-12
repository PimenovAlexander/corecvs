#include "calibrationWidget.h"
#include "ui_calibrationWidget.h"
#include "opencv2/core.hpp"
#include "imageForCalibrationWidget.h"
#include "iostream"
#include <opencv2/opencv.hpp>
#include <QDir>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <thread>
#include <mutex>
#include <QTimer>

CalibrationWidget::CalibrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibrationWidget)
{
    ui->setupUi(this);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    updateVideo();
    addImage();
    QTimer::singleShot(38,this,SLOT(updateImage()));
}

CalibrationWidget::~CalibrationWidget()
{
    delete ui;
}

void CalibrationWidget::updateImage()
{
    for (int i=0;i<widgets.size();i++)
    {
        widgets[i]->updateMicroImage();
    }
    QTimer::singleShot(38,this,SLOT(updateImage()));
}

void CalibrationWidget::addImage()
{
    QLayout *layout = ui->imageBox->layout();
    cv::Mat defaultIm;
    ImageForCalibrationWidget *widget = new ImageForCalibrationWidget();
    connect(widget,SIGNAL(approved()), this , SLOT(addImage()));
    connect(widget,SIGNAL(approved()), this , SLOT(updateStartButton()));
    connect(widget,SIGNAL(closed(int)), this , SLOT(updateStartButtonAndRemoveWidget(int)));
    widget->setId(widgetCounter);
    widgetCounter++;
    vectorMutex.lock();
    widgets.push_back(widget);
    vectorMutex.unlock();
    layout->addWidget(widget);
}

void CalibrationWidget::updateStartButton()
{
    vectorMutex.lock();
    if (widgets.size()>4)
    {
        ui->startButton->setEnabled(true);
    }
    else {
        ui->startButton->setEnabled(false);
    }
    vectorMutex.unlock();
}

void CalibrationWidget::updateStartButtonAndRemoveWidget(int k)
{
    vectorMutex.lock();
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
        ui->startButton->setEnabled(true);
    }
    else {
        ui->startButton->setEnabled(false);
    }
    vectorMutex.unlock();
}

void CalibrationWidget::addImage(cv::Mat)
{
    vectorMutex.lock();
    QLayout *layout = ui->imageBox->layout();
    ImageForCalibrationWidget *widget = new ImageForCalibrationWidget();
    widgets.push_back(widget);
    layout->addWidget(widget);
    vectorMutex.unlock();

}

void CalibrationWidget::startCalibration()
{
    setCapute(false);
    vectorMutex.lock();                                 //thread should be stopped here, but who knows
    std::cout<<"start"<<std::endl;
    for (int i=0; i<widgets.size();i++)
    {
        widgets[i]->lockButtons();
    }
    vectorMutex.unlock();
}

void CalibrationWidget::startRecording()
{
    if (!threadRunning)
    {
        std::thread thr([this]()
        {
        int numCornersHor = 0;
        numCornersHor = ui->widthLine->text().toInt();
        int numCornersVer = 0;
        numCornersVer = ui->heightLine->text().toInt();
        if (numCornersHor * numCornersVer != 0 && cameraNumber!=-1)
        {
            int numSquares = numCornersHor * numCornersVer;
            cv::Size board_sz = cv::Size(numCornersHor, numCornersVer);
            cv::VideoCapture capture = cv::VideoCapture(cameraNumber);

            cv::Mat image;
            cv::Mat gray_image;
            //capture >> image;

            std::vector<cv::Point2f> corners;
            setCapute(true);
            //addImage();
            while (getCapute())
            {
                capture >> image;
                cvtColor(image, gray_image, CV_RGB2GRAY);
                bool found = findChessboardCorners(image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
                if(found)
                {
                    //std::cout<<"TRUE----------------------------------------------------------------------------------------------------"<<std::endl;
                    //std::cout<<corners<<std::endl;
                    cornerSubPix( gray_image, corners, cv::Size(11,11),
                                                cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
                    //std::cout<<"TRUE2222"<<std::endl;
                    cv::drawChessboardCorners(gray_image, board_sz, corners, found);
                    vectorMutex.lock();
                    widgets[widgets.size()-1]->setImage(&image);
                    vectorMutex.unlock();
                }
                if (!found)
                {
                    std::cout<<"FALSE"<<std::endl;
                }
                imshow("gray", gray_image);
            }
        }
        });
        thr.detach();
        threadRunning = true;
    }
}

void CalibrationWidget::updateVideo()
{
    QDir DevDir=*new QDir("/dev","video*",QDir::Name,QDir::System);
    ui->videoBox->clear();
    ui->videoBox->addItems(DevDir.entryList());
}



void CalibrationWidget::on_videoBox_currentIndexChanged(int index)         //if linux will drop some numbers it will not work
{
    cameraNumber = index;
}

void CalibrationWidget::setCapute(bool b)
{
    caputingMutex.lock();
    caputing = b;
    caputingMutex.unlock();
}

bool CalibrationWidget::getCapute()
{
    bool b;
    caputingMutex.lock();
    b = caputing;
    caputingMutex.unlock();
    return b;
}
