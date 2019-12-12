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
#include <buffers/bufferFactory.h>
#include <cameracalibration/cameraModel.h>
#include <reflection/jsonPrinter.h>

#include "wrappers/opencv/openCVTools.h"

#include "core/utils/global.h"
#include "core/utils/utils.h"

using std::vector;

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


    connect(ui->captureImagePushButton, SIGNAL(released()), this, SLOT(captureImage()));
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(ui->loadButton, SIGNAL(released()), this, SLOT(load()));
    connect(ui->saveButton, SIGNAL(released()), this, SLOT(save()));


    connect(ui->startCalibrationButton, SIGNAL(released()), this, SLOT(startCalibration()));
    //addImage();
    //QTimer::singleShot(38,this,SLOT(updateImage()));
}

CalibrationWidget::~CalibrationWidget()
{
    SYNC_PRINT(("CalibrationWidget::~CalibrationWidget(): called\n"));
    delete ui;
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
    if (mInterface->supportPause()) {
        ui->pauseRecordingButton->setEnabled(true);
    }

    mCameraParametersWidget.setCaptureInterface(mInterface);

    mInterface->startCapture();
    QTimer::singleShot(50, this, SLOT(newFrameRequset()));

}

void CalibrationWidget::pauseRecording()
{
    if (mInterface != NULL)
    {
        mInterface->pauseCapture();
    }
}

/* Camera controls */
void CalibrationWidget::stopRecording()
{
    if (mInterface == NULL)
    {
        return;
    }
    delete_safe(mInterface);
    ui->startRecordingButton->setEnabled(true);
    ui->stopRecordingButton->setEnabled(false);
}


void CalibrationWidget::newFrameRequset()
{
    //SYNC_PRINT(("CalibrationWidget::newFrameRequset(): requesting frame\n"));
    if (mInterface == NULL) {
        return;
    }
    ImageCaptureInterface::FramePair pair = mInterface->getFrameRGB24();
    delete_safe(currentImage);

    currentImage = pair.rgbBufferLeft();
    pair.setRgbBufferLeft(NULL);
    pair.freeBuffers();

    if (currentImage != NULL)
      ui->imageWidget->setImage(QSharedPointer<QImage>(new RGB24Image(currentImage)));

    QTimer::singleShot(50, this, SLOT(newFrameRequset()));
}

void CalibrationWidget::captureImage()
{
    SYNC_PRINT(("CalibrationWidget::captureImage(): called \n"));
    if (currentImage == NULL)
    {
        SYNC_PRINT(("CalibrationWidget::captureImage(): current image NULL\n"));
    } else {
        SYNC_PRINT(("CalibrationWidget::captureImage(): [%d x %d]\n", currentImage->h, currentImage->w));
    }
    addImage(currentImage);
}

void CalibrationWidget::load()
{
    SYNC_PRINT(("CalibrationWidget::load(): called \n"));
    for (size_t i = 0; ; i++)
    {
        std::ostringstream oss;
        oss << "calib" << i << ".png";
        RGB24Buffer *rgb24image = BufferFactory::getInstance()->loadRGB24Bitmap(oss.str());
        if (rgb24image == NULL) {
            return;
        }
        addImage(rgb24image);
    }
}

void CalibrationWidget::save()
{
    SYNC_PRINT(("CalibrationWidget::save(): called \n"));
    for (size_t i = 0; i < widgets.size(); i++)
    {
        std::ostringstream oss;
        oss << "calib" << i << ".png";
        RGB24Buffer *rgb24image = widgets[i]->rgb24Image;
        BufferFactory::getInstance()->saveRGB24Bitmap(rgb24image, oss.str());
    }
}

void CalibrationWidget::updateImage()
{
    for (size_t i = 0; i < widgets.size(); i++)
    {
        widgets[i]->updateThumbnail();
    }
    QTimer::singleShot(38,this,SLOT(updateImage()));
}

void CalibrationWidget::addImage(RGB24Buffer *image)
{
    QLayout *layout = ui->imageBox->layout();
    ImageForCalibrationWidget *widget = new ImageForCalibrationWidget();
    //connect(widget,SIGNAL(approved()), this , SLOT(addImage()));
    //connect(widget,SIGNAL(approved()), this , SLOT(updateStartButton()));
    //connect(widget,SIGNAL(closed(int)), this , SLOT(updateStartButtonAndRemoveWidget(int)));

    widgets.push_back(widget);
    layout->addWidget(widget);
    widget->setImage(new RGB24Buffer(image));
    updateStartButton();
}

void CalibrationWidget::updateStartButton()
{
    if (widgets.size() > 4)
    {
        ui->startCalibrationButton->setEnabled(true);
    }
    else {
        ui->startCalibrationButton->setEnabled(false);
    }
}


void CalibrationWidget::calibrate()
{
    SYNC_PRINT(("CalibrationWidget::calibrate():called "));
    int boardWidth = ui->widthSpinBox->value();
    int boardHeight = ui->heightSpinBox->value();

    cv::Size board_sz = cv::Size(boardWidth, boardHeight);

    vector<vector<cv::Point3f>> object_points;
    vector<vector<cv::Point2f>> image_points;

    vector<cv::Point2f> corners;

    cv::Size imageSize;

    std::vector<cv::Point3f> obj;
    for(int i = 0; i < boardHeight; i++) {
        for(int j = 0; j < boardWidth; j++) {
            obj.push_back(cv::Point3d((float)j * 20, (float)i * 20, 0.0f));
        }
    }

    std::cout << widgets.size() - 1 << std::endl;

    for (size_t i = 0; i < widgets.size() - 1; i++)
    {        
        RGB24Buffer *rgb24image = widgets[i]->rgb24Image;
        cv::Mat image = OpenCVTools::getCVMatFromRGB24Buffer(rgb24image);
        cv::Mat gray_image;

        SYNC_PRINT(("CalibrationWidget::calibrate():processing %d [%d %d]\n", i, image.cols, image.rows));
        cvtColor(image, gray_image, CV_RGB2GRAY);
        bool found = findChessboardCorners(image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

        SYNC_PRINT(("CalibrationWidget::calibrate():board found : %s\n", found ? "yes" : "no"));

        if (found)
        {
            imageSize = image.size();

            cornerSubPix( gray_image, corners, cv::Size(11,11),
                                cv::Size(-1,-1), cv::TermCriteria( cv::TermCriteria::EPS+cv::TermCriteria::COUNT, 30, 0.1 ));
            image_points .push_back(corners);
            object_points.push_back(obj);
        }
    }

    cv::Mat intrinsic = cv::Mat(3, 3, CV_32FC1);
    cv::Mat distCoeffs;
    vector<cv::Mat> rvecs;
    vector<cv::Mat> tvecs;

    intrinsic.ptr<float>(0)[0] = 1;
    intrinsic.ptr<float>(1)[1] = 1;

    SYNC_PRINT(("Calling actual calibration image\n"));
    calibrateCamera(object_points, image_points, imageSize, intrinsic, distCoeffs, rvecs, tvecs);
    SYNC_PRINT(("Actual calibration finished\n"));

    cv::Mat imageUndistorted;
    intrinsic.copyTo(globalIntrinsic);
    distCoeffs.copyTo(globalDistCoeffs);

    cout << "Intrinsics:" << globalIntrinsic  << endl;
    cout << "Distorion :" << globalDistCoeffs << endl;

    /*we will try to convert it to CameraModel */

    CameraModel model;
    model.setLocation(Affine3DQ::Identity());
    PinholeCameraIntrinsics *p = model.getPinhole();

    p->setFocal    (Vector2dd(globalIntrinsic.at<double>(0,0), globalIntrinsic.at<double>(1,1)));
    p->setPrincipal(Vector2dd(globalIntrinsic.at<double>(0,2), globalIntrinsic.at<double>(1,2)));
    p->setSize     (Vector2dd(imageSize.width, imageSize.height));
    p->setDistortedSize(p->size());
    p->setSkew(0.0);
    p->offset = Vector2dd::Zero();

    model.distortion.setPrincipalPoint(p->principal());
    model.distortion.setTangentialX(globalDistCoeffs.at<double>(2));
    model.distortion.setTangentialY(globalDistCoeffs.at<double>(3));

    double k1 = globalDistCoeffs.at<double>(0);
    double k2 = globalDistCoeffs.at<double>(1);
    double k3 = globalDistCoeffs.at<double>(4);

    model.distortion.setKoeff(
      { 0, k1, 0, k2, 0, k3});

    model.distortion.setNormalizingFocal(p->size().y() / 2);

    JSONPrinter printer;
    printer.visit(model, "camera");

    JSONPrinter printer1("camera.json");
    printer1.visit(model, "camera");


    //setShowingBool(true);
    //ui->stopShowing->setEnabled(true);
    //ui->saveCalib->setEnabled(true);
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
    SYNC_PRINT(("CalibrationWidget::startCalibration():called "));
    calibrate();
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
