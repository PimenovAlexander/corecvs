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

ScannerDialog::ScannerDialog()
    : BaseHostDialog(),
      mIsRecording(false),
      mScannerControlWidget(NULL)
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

    mScannerControlWidget = new ScannerControlWidget(this, true, UI_NAME_SCANNER);
    dockWidget()->layout()->addWidget(mScannerControlWidget);
    mSaveableWidgets.push_back(mScannerControlWidget);


    connect(mScannerControlWidget->ui()->choosePathButton, SIGNAL(clicked()), this, SLOT(openPathSelectDialog()));
    connect(mScannerControlWidget->ui()->recStartButton, SIGNAL(clicked()), this, SLOT(toggleRecording()));

}

void ScannerDialog::createCalculator()
{
    ParametersMapperScanner *mapper = new ParametersMapperScanner();

    mapper->setScannerControlWidget(mScannerControlWidget);
    mapper->setBaseParametersControlWidget(mBaseControlWidget);
    mapper->setPresentationParametersControlWidget(mPresentationControlWidget);

    connect(mapper, SIGNAL(scannerParamsChanged(QSharedPointer<Scanner>))
            , this, SLOT(scannerControlParametersChanged(QSharedPointer<Scanner>)));

  /*  connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , this, SLOT(baseControlParametersChanged(QSharedPointer<Base>)));*/

    mCalculator = new ScannerThread();

    connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , static_cast<ScannerThread*>(mCalculator)
            , SLOT(baseControlParametersChanged(QSharedPointer<BaseParameters>)));
    connect(mapper, SIGNAL(scannerParamsChanged(QSharedPointer<Scanner>))
            , static_cast<ScannerThread*>(mCalculator)
            , SLOT(scannerControlParametersChanged(QSharedPointer<Scanner>)));
    connect(mapper, SIGNAL(presentationParametersParamsChanged(QSharedPointer<PresentationParameters>))
            , static_cast<ScannerThread*>(mCalculator)
            , SLOT(presentationControlParametersChanged(QSharedPointer<PresentationParameters>)));

    mapper->paramsChanged();

    mParamsMapper = mapper;

    connect(this, SIGNAL(recordingTriggered()), mCalculator, SLOT(toggleRecording()), Qt::QueuedConnection);

    connect(mScannerControlWidget->ui()->recRestartButton, SIGNAL(released()), mCalculator, SLOT(resetRecording()), Qt::QueuedConnection);
    connect(mScannerControlWidget->ui()->recPauseButton, SIGNAL(released()), mCalculator, SLOT(toggleRecording()), Qt::QueuedConnection);

    connect(mCalculator, SIGNAL(recordingStateChanged(ScannerThread::RecordingState)), this,
            SLOT(recordingStateChanged(ScannerThread::RecordingState)), Qt::QueuedConnection);

    connect(mCalculator, SIGNAL(errorMessage(QString)),
            this,        SLOT  (errorMessage(QString)), Qt::BlockingQueuedConnection);

}


void ScannerDialog::scannerControlParametersChanged(QSharedPointer<Scanner> params)
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

    if (dir_path.length() > 0)
    {
        mScannerControlWidget->ui()->pathEdit->setText(dir_path);
    }
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
    switch (state)
    {
        case ScannerThread::StateRecordingActive:
        {
            mIsRecording = true;
            mBaseControlWidget->setEnabled(false);
            mScannerControlWidget->ui()->recStartButton->setEnabled(false);
            mScannerControlWidget->ui()->recPauseButton->setEnabled(true);
            mScannerControlWidget->ui()->recRestartButton->setEnabled(true);
            mScannerControlWidget->ui()->pathEdit->setEnabled(false);
            mScannerControlWidget->ui()->fileTemplateEdit->setEnabled(false);
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
            mScannerControlWidget->ui()->recStartButton->setChecked(false);
            mScannerControlWidget->ui()->recStartButton->setEnabled(true);
            mScannerControlWidget->ui()->recPauseButton->setChecked(false);
            mScannerControlWidget->ui()->recPauseButton->setEnabled(false);
            mScannerControlWidget->ui()->recRestartButton->setEnabled(false);
            mScannerControlWidget->ui()->pathEdit->setEnabled(true);
            mScannerControlWidget->ui()->fileTemplateEdit->setEnabled(true);
            break;
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

        if (mIsRecording)
            mScannerControlWidget->ui()->frameCountLabel->setText(QString("Frame (frame pairs) written: %1").arg(fod->frameCount));


//        fod->mMainImage.print();

        if (i == eventList.size() - 1) {
            mImage = QSharedPointer<QImage>(new QImage(fod->mMainImage.width(), fod->mMainImage.height(),  QImage::Format_RGB32));
            fod->mMainImage.drawImage(mImage.data());
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
