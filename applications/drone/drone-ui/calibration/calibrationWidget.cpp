#include "calibrationWidget.h"
#include "ui_calibrationWidget.h"
#include "imageForCalibrationWidget.h"
#include <iostream>
#include <thread>
#include <mutex>


#include <QDir>
#include <QTimer>
#include <QFileDialog>
#include <imageCaptureInterfaceQt.h>
#include <g12Image.h>


#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include "core/utils/global.h"
#include "core/utils/utils.h"

CalibrationWidget::CalibrationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibrationWidget)
{
    ui->setupUi(this);
    /* Cameras */
    mInputSelector.setInputString("v4l2:/dev/video0:1/5");

    connect(ui->inputSettingsButton  , SIGNAL(released()), this, SLOT(showInputSettings()));
    connect(ui->camerasSettingsButton, SIGNAL(released()), this, SLOT(showCameraSettings()));

    connect(ui->startRecordingButton, SIGNAL(released()), this, SLOT(startRecording()));
    connect(ui->stopRecordingButton , SIGNAL(released()), this, SLOT(stopRecording()) );
    connect(ui->pauseRecordingButton, SIGNAL(released()), this, SLOT(pauseRecording()));


    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //addImage();
    //QTimer::singleShot(38,this,SLOT(updateImage()));
}

CalibrationWidget::~CalibrationWidget()
{
    std::cout<<"destructor"<<std::endl;
    setCapute(false);
    threadRunning = false;
    delete ui;
}

/* Camera controls */
void CalibrationWidget::stopRecording()
{

}

void CalibrationWidget::startRecording()
{
    std::string str = mInputSelector.getInputString().toStdString();
    mInterface = ImageCaptureInterfaceQtFactory::fabric(str, true);
    if (mInterface == NULL)
        return;

    SYNC_PRINT(("CalibrationWidget::startRecording(): initialising capture...\n"));
    ImageCaptureInterface::CapErrorCode returnCode = mInterface->initCapture();
    SYNC_PRINT(("CalibrationWidget::startRecording(): initialising capture returing %d\n", returnCode));

    if (returnCode == ImageCaptureInterface::FAILURE)
    {
        SYNC_PRINT(("Can't open\n"));
        return;
    }

    /*Ok we seem to have started recording */
    ui->startRecordingButton->setEnabled(false);
    ui->stopRecordingButton->setEnabled(true);
    ui->pauseRecordingButton->setEnabled(true);

    mCameraParametersWidget.setCaptureInterface(mInterface);

    mInterface->startCapture();
    QTimer::singleShot(50, this, SLOT(newFrameRequset()));

}

void CalibrationWidget::pauseRecording()
{

}

void CalibrationWidget::newFrameRequset()
{
    SYNC_PRINT(("CalibrationWidget::newFrameRequset(): requesting frame\n"));
    ImageCaptureInterface::FramePair pair = mInterface->getFrameRGB24();
    RGB24Buffer * result = pair.rgbBufferLeft();
    pair.setRgbBufferLeft(NULL);
    pair.freeBuffers();

    int numCornersHor = ui->widthSpinBox->value();
    int numCornersVer = ui->heightSpinBox->value();

    int numSquares = numCornersHor * numCornersVer;

    cv::Size board_sz = cv::Size(numCornersHor, numCornersVer);

    cv::Mat image;
    cv::Mat gray_image;

    std::vector<cv::Point2f> corners;

    bool found = findChessboardCorners(image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

    if(found)
    {
        cvtColor(image, gray_image, CV_RGB2GRAY);
        cornerSubPix( gray_image, corners, cv::Size(11,11),
                                    cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
        cv::drawChessboardCorners(gray_image, board_sz, corners, found);       
    }

    if (result != NULL)
    ui->imageWidget->setImage(QSharedPointer<QImage>(new RGB24Image(result)));

    QTimer::singleShot(50, this, SLOT(newFrameRequset()));
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

void CalibrationWidget::calibrate()
{
    int numBoards = widgets.size();
    int numCornersHor = 9;
    int numCornersVer = 7;

    int numSquares = numCornersHor * numCornersVer;
    cv::Size board_sz = cv::Size(numCornersHor, numCornersVer);

    std::vector<std::vector<cv::Point3f>> object_points;
    std::vector<std::vector<cv::Point2f>> image_points;

    std::vector<cv::Point2f> corners;
    int successes=0;

    cv::Mat image;
    cv::Mat gray_image;

    std::vector<cv::Point3f> obj;
    for(int j=0;j<numSquares;j++)
        obj.push_back(cv::Point3d(j/numCornersHor, j%numCornersHor, 0.0f));
    std::cout<<widgets.size()-1<<std::endl;
    for (int i=0;i<widgets.size()-1;i++)
    {
        image = widgets[i]->getImage();
        cvtColor(image, gray_image, CV_RGB2GRAY);
        findChessboardCorners(image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
        cornerSubPix( gray_image, corners, cv::Size(11,11),
                                cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
        image_points.push_back(corners);
        object_points.push_back(obj);
    }

    cv::Mat intrinsic = cv::Mat(3, 3, CV_32FC1);
    cv::Mat distCoeffs;
    std::vector<cv::Mat> rvecs;
    std::vector<cv::Mat> tvecs;

    intrinsic.ptr<float>(0)[0] = 1;
    intrinsic.ptr<float>(1)[1] = 1;

    calibrateCamera(object_points, image_points, image.size(), intrinsic, distCoeffs, rvecs, tvecs);

    cv::Mat imageUndistorted;
    intrinsic.copyTo(globalIntrinsic);
    distCoeffs.copyTo(globalDistCoeffs);
    setShowingBool(true);
    ui->stopShowing->setEnabled(true);
    ui->saveCalib->setEnabled(true);
    std::thread thr([this]()
    {
        cv::Mat image;
        cv::Mat imageUndistorted;
        cv::VideoCapture capture = cv::VideoCapture(cameraNumber);
        std::cout<<globalIntrinsic<<std::endl;
        std::cout<<globalDistCoeffs<<std::endl;
        while(getShowingBool())
        {
            capture >> image;
            undistort(image, imageUndistorted, globalIntrinsic, globalDistCoeffs);

            imshow("win1", image);
            imshow("win2", imageUndistorted);
            cv::waitKey(1);
        }
        std::cout<<"Show thread is over"<<std::endl;
    });
    thr.detach();
    std::cout<<"here 4"<<std::endl;
}

bool CalibrationWidget::getShowingBool()
{
    showingMutex.lock();
    bool b = showingBool;
    showingMutex.unlock();
    return b;
}

void CalibrationWidget::stopShowing()
{
    setShowingBool(false);
    ui->stopShowing->setEnabled(false);
    ui->saveCalib->setEnabled(false);
}
void CalibrationWidget::setShowingBool(bool b)
{
    showingMutex.lock();
    showingBool = b;
    showingMutex.unlock();
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

    calibrate();
    vectorMutex.unlock();
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

void CalibrationWidget::saveMatrix()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Calibration Parametrs"),"",tr("Calibration Parametrs (*.clb);; All Files (*)"));
    if (!fileName.isEmpty())
    {
        fileName += ".yml";
        cv::FileStorage fs(fileName.toStdString(),cv::FileStorage::WRITE);
        fs <<"intrinsic"<<globalIntrinsic;
        fs <<"distCoeffs"<<globalDistCoeffs;
    }
}

void CalibrationWidget::showInputSettings()
{
    mInputSelector.show();
    mInputSelector.raise();
}

void CalibrationWidget::showCameraSettings()
{
    mCameraParametersWidget.show();
    mCameraParametersWidget.raise();
}
