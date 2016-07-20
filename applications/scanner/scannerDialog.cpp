/**
 * \file scannerDialog.cpp
 * \brief Implements frame recording dialog based on BaseHostDialog
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include "scannerDialog.h"

#include <stdio.h>
#include <QLayout>
#include <QDir>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include "parametersMapper/parametersMapperScanner.h"
#include "scannerThread.h"
#include "mesh3DScene.h"


ScannerDialog::ScannerDialog()
    : BaseHostDialog(),
      mIsScanning(false),
      mScannerParametersControlWidget(NULL)
{
//    this->resize(this->width(), this->height() - 65);

}

ScannerDialog::ScannerDialog(QString scannerPath)
{
    ScannerDialog();
    scanCtrl.openDevice(scannerPath);
}

ScannerDialog::~ScannerDialog()
{
    terminateCalculator();

    cleanupEventQueue();
}

void ScannerDialog::initParameterWidgets()
{
    BaseHostDialog::initParameterWidgets();

    mScannerParametersControlWidget = new ScannerParametersControlWidgetAdv(this, true, UI_NAME_SCANNER);
    dockWidget()->layout()->addWidget(mScannerParametersControlWidget);
    mSaveableWidgets.push_back(mScannerParametersControlWidget);

    cloud = static_cast<CloudViewDialog *>(createAdditionalWindow("3d view", oglWindow, QIcon()));
    graph = static_cast<GraphPlotDialog *>(createAdditionalWindow("Graph view", graphWindow, QIcon()));
    addImage = static_cast<AdvancedImageWidget *>(createAdditionalWindow("Addition", imageWindow, QIcon()));
    brightImage = static_cast<AdvancedImageWidget *>(createAdditionalWindow("Brightness", imageWindow, QIcon()));
    channelImage = static_cast<AdvancedImageWidget *>(createAdditionalWindow("Channel", imageWindow, QIcon(":/new/colors/colors/color_wheel.png")));
    cornerImage = static_cast<AdvancedImageWidget *>(createAdditionalWindow("Corner", imageWindow, QIcon()));


    //connect(mScannerParametersControlWidget->ui()->choosePathButton, SIGNAL(clicked()), this, SLOT(openPathSelectDialog()));
    //connect(mScannerParametersControlWidget->ui()->recStartButton, SIGNAL(clicked()), this, SLOT(toggleRecording()));

}

void ScannerDialog::createCalculator()
{
    ParametersMapperScanner *mapper = new ParametersMapperScanner();

    mapper->setScannerParametersControlWidget(mScannerParametersControlWidget);
    mapper->setBaseParametersControlWidget(mBaseControlWidget);
    mapper->setPresentationParametersControlWidget(mPresentationControlWidget);

    connect(mapper, SIGNAL(scannerParametersParamsChanged(QSharedPointer<ScannerParameters>))
            , this, SLOT(scannerControlParametersChanged(QSharedPointer<ScannerParameters>)));

  /*  connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , this, SLOT(baseControlParametersChanged(QSharedPointer<Base>)));*/

    ScannerThread *calculatorTyped = new ScannerThread();
    mCalculator = calculatorTyped;


    connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , calculatorTyped
            , SLOT(baseControlParametersChanged(QSharedPointer<BaseParameters>)));
    connect(mapper, SIGNAL(scannerParametersParamsChanged(QSharedPointer<ScannerParameters>))
            , calculatorTyped
            , SLOT(scannerControlParametersChanged(QSharedPointer<ScannerParameters>)));
    connect(mapper, SIGNAL(presentationParametersParamsChanged(QSharedPointer<PresentationParameters>))
            , calculatorTyped
            , SLOT(presentationControlParametersChanged(QSharedPointer<PresentationParameters>)));

    mapper->paramsChanged();

    mParamsMapper = mapper;

//    connect(this, SIGNAL(recordingTriggered()), calculatorTyped , SLOT(toggleScanning()), Qt::QueuedConnection);

    connect(mScannerParametersControlWidget->startButton, SIGNAL(released()),
                     calculatorTyped, SLOT(toggleScanning()));



    /*connect(mCalculator, SIGNAL(sctateChanged(ScannerThread::RecordingState)), this,
            SLOT(recordingStateChanged(ScannerThread::RecordingState)), Qt::QueuedConnection);

    connect(mCalculator, SIGNAL(errorMessage(QString)),
            this,        SLOT  (errorMessage(QString)), Qt::BlockingQueuedConnection);
    */
    connect(calculatorTyped, SIGNAL(scanningStateChanged(ScannerThread::ScanningState)), this,
                SLOT(scanningStateChanged(ScannerThread::ScanningState)), Qt::QueuedConnection);

}


void ScannerDialog::scannerControlParametersChanged(QSharedPointer<ScannerParameters> params)
{
    if (!params)
        return;

    mScannerControlParams = params;
}

void ScannerDialog::connectFinishedRecalculation()
{
    ScannerThread *calculator = dynamic_cast<ScannerThread *>(mCalculator);

    if (calculator)
    {
        connect(calculator, SIGNAL(processingFinished(AbstractOutputData *)),
                this, SLOT(finishedRecalculation(AbstractOutputData *)), Qt::QueuedConnection);
        play();
    }
}

void ScannerDialog::openPathSelectDialog()
{
    QString dir_path = QFileDialog::getExistingDirectory( this,
                                                          "Select target directory",
                                                          QDir::homePath()
                                                          );

    /*if (dir_path.length() > 0)
    {
        mScannerParametersControlWidget->ui()->pathEdit->setText(dir_path);
    }*/
}

/*void ScannerDialog::toggleRecording()
{
    emit recordingTriggered();
}

void ScannerDialog::resetRecording()
{
    emit recordingReset();
}*/

void ScannerDialog::scanningStateChanged(ScannerThread::ScanningState state)
{
#if 0
    switch (state)
    {
        case ScannerThread::StateRecordingActive:
        {
            mIsRecording = true;
            mBaseControlWidget->setEnabled(false);
            mScannerParametersControlWidget->ui()->recStartButton->setEnabled(false);
            mScannerParametersControlWidget->ui()->recPauseButton->setEnabled(true);
            mScannerParametersControlWidget->ui()->recRestartButton->setEnabled(true);
            mScannerParametersControlWidget->ui()->pathEdit->setEnabled(false);
            mScannerParametersControlWidget->ui()->fileTemplateEdit->setEnabled(false);
            break;
        }
        case ScannerThread::StateRecordingPaused:
        {
            break;
        }
        case ScannerThread::StateRecordingReset:
        case ScannerThread::StateRecordingFailure:
        {
            mIsRecording = false;
            mBaseControlWidget->setEnabled(true);
            mScannerParametersControlWidget->ui()->recStartButton->setChecked(false);
            mScannerParametersControlWidget->ui()->recStartButton->setEnabled(true);
            mScannerParametersControlWidget->ui()->recPauseButton->setChecked(false);
            mScannerParametersControlWidget->ui()->recPauseButton->setEnabled(false);
            mScannerParametersControlWidget->ui()->recRestartButton->setEnabled(false);
            mScannerParametersControlWidget->ui()->pathEdit->setEnabled(true);
            mScannerParametersControlWidget->ui()->fileTemplateEdit->setEnabled(true);
            break;
        }
    }
#endif

    switch (state)
    {
        case ScannerThread::HOMEING:
        {
            scanCtrl.laserOff();
            scanCtrl.home();
            while(scanCtrl.getPos());


            scanCtrl.laserOn();
            sleep(1);
            scanCtrl.laserOff();

            scanCtrl.laserOn();
            sleep(1);
            scanCtrl.laserOff();

            scanCtrl.laserOn();
            sleep(1);
            scanCtrl.laserOff();

            emit scanningStateChanged(ScannerThread::IDLE);
            break;
        }

        case ScannerThread::SCANNING:
        {
            mIsScanning = true;
            scanCtrl.laserOn();
            scanCtrl.step(10000);

            //kokokokoko

            scanCtrl.laserOff();
            emit scanningStateChanged(ScannerThread::PAUSED);
        }
        case ScannerThread::PAUSED:
        {

        }

    }
}

void ScannerDialog::processResult()
{

    for (unsigned i = 0; i < eventList.size(); i++)
    {
        ScannerOutputData *fod = dynamic_cast<ScannerOutputData*>(eventList[i]);
        if (fod == NULL)
            break;

        mStatsDialog.addStats(fod->stats);

       /* if (mIsRecording)
            mScannerParametersControlWidget->ui()->frameCountLabel->setText(QString("Frame (frame pairs) written: %1").arg(fod->frameCount));
        */

//        fod->mMainImage.print();

        if (i == eventList.size() - 1) {
            mImage = QSharedPointer<QImage>(new QImage(fod->mMainImage.width(), fod->mMainImage.height(),  QImage::Format_RGB32));
            fod->mMainImage.drawImage(mImage.data());

            QSharedPointer<Mesh3DScene> scene(new Mesh3DScene());
            scene->switchColor(true);
            scene->add(fod->outputMesh, true);

            cloud->setNewScenePointer(scene);


            for (size_t i = 0; i < fod->cut.size(); i++)
            {
                graph->addGraphPoint("R", fod->cut[i]);
                graph->addGraphPoint("R_conv", fod->cutConvolution[i]);
            }
            graph->update();
            if (fod->convolution) addImage    ->setImage(QSharedPointer<QImage>(new RGB24Image(fod->convolution)));
            if (fod->channel)     channelImage->setImage(QSharedPointer<QImage>(new G8Image(fod->channel)));
            if (fod->brightness)  brightImage ->setImage(QSharedPointer<QImage>(new G8Image(fod->brightness)));
            if (fod->corners)     cornerImage ->setImage(QSharedPointer<QImage>(new G8Image(fod->corners)));
        }

        delete fod;
    }
    updateWidgets();
}

void ScannerDialog::errorMessage(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}
