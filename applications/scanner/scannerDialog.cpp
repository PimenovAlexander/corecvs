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

#include "mesh3DScene.h"


ScannerDialog::ScannerDialog()
    : BaseHostDialog(),
      mIsRecording(false),
      mScannerParametersControlWidget(NULL)
{
//    this->resize(this->width(), this->height() - 65);

}

ScannerDialog::~ScannerDialog()
{
    terminateCalculator();

    cleanupEventQueue();
}

void ScannerDialog::initParameterWidgets()
{
    BaseHostDialog::initParameterWidgets();

    mScannerParametersControlWidget = new ScannerParametersControlWidget(this, true, UI_NAME_SCANNER);
    dockWidget()->layout()->addWidget(mScannerParametersControlWidget);
    mSaveableWidgets.push_back(mScannerParametersControlWidget);


    cloud = static_cast<CloudViewDialog *>(createAdditionalWindow("3d view", oglWindow, QIcon()));
    graph = static_cast<GraphPlotDialog *>(createAdditionalWindow("Graph view", graphWindow, QIcon()));
    addImage = static_cast<AdvancedImageWidget *>(createAdditionalWindow("Addition", imageWindow, QIcon()));
    channelImage = static_cast<AdvancedImageWidget *>(createAdditionalWindow("Channel", imageWindow, QIcon(":/new/colors/colors/color_wheel.png")));



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

    mCalculator = new ScannerThread();

    connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , static_cast<ScannerThread*>(mCalculator)
            , SLOT(baseControlParametersChanged(QSharedPointer<BaseParameters>)));
    connect(mapper, SIGNAL(scannerParametersParamsChanged(QSharedPointer<ScannerParameters>))
            , static_cast<ScannerThread*>(mCalculator)
            , SLOT(scannerControlParametersChanged(QSharedPointer<ScannerParameters>)));
    connect(mapper, SIGNAL(presentationParametersParamsChanged(QSharedPointer<PresentationParameters>))
            , static_cast<ScannerThread*>(mCalculator)
            , SLOT(presentationControlParametersChanged(QSharedPointer<PresentationParameters>)));

    mapper->paramsChanged();

    mParamsMapper = mapper;

    connect(this, SIGNAL(recordingTriggered()), mCalculator, SLOT(toggleRecording()), Qt::QueuedConnection);

    //connect(mScannerParametersControlWidget->ui()->recRestartButton, SIGNAL(released()), mCalculator, SLOT(resetRecording()), Qt::QueuedConnection);
    //connect(mScannerParametersControlWidget->ui()->recPauseButton, SIGNAL(released()), mCalculator, SLOT(toggleRecording()), Qt::QueuedConnection);

    connect(mCalculator, SIGNAL(recordingStateChanged(ScannerThread::RecordingState)), this,
            SLOT(recordingStateChanged(ScannerThread::RecordingState)), Qt::QueuedConnection);

    connect(mCalculator, SIGNAL(errorMessage(QString)),
            this,        SLOT  (errorMessage(QString)), Qt::BlockingQueuedConnection);

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

void ScannerDialog::toggleRecording()
{
    emit recordingTriggered();
}

void ScannerDialog::resetRecording()
{
    emit recordingReset();
}

void ScannerDialog::recordingStateChanged(ScannerThread::RecordingState state)
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
            addImage    ->setImage(QSharedPointer<QImage>(new RGB24Image(fod->convolution)));
            channelImage->setImage(QSharedPointer<QImage>(new G8Image(fod->channel)));
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
