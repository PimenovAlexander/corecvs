/**
 * \file recorderDialog.cpp
 * \brief Implements frame recording dialog based on BaseHostDialog
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include "recorderDialog.h"

#include <stdio.h>
#include <QLayout>
#include <QDir>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include "parametersMapper/parametersMapperRecorder.h"

RecorderDialog::RecorderDialog()
    : BaseHostDialog(),
      mIsRecording(false),
      mRecorderControlWidget(NULL)
{
//    this->resize(this->width(), this->height() - 65);

}

RecorderDialog::~RecorderDialog()
{
    terminateCalculator();

    cleanupEventQueue();
}

void RecorderDialog::initParameterWidgets()
{
    BaseHostDialog::initParameterWidgets();

    mRecorderControlWidget = new RecorderControlWidget(this, true, UI_NAME_RECORDER);
    dockWidget()->layout()->addWidget(mRecorderControlWidget);
    mSaveableWidgets.push_back(mRecorderControlWidget);


    connect(mRecorderControlWidget->ui()->choosePathButton, SIGNAL(clicked()), this, SLOT(openPathSelectDialog()));
    connect(mRecorderControlWidget->ui()->recStartButton, SIGNAL(clicked()), this, SLOT(toggleRecording()));

}

void RecorderDialog::createCalculator()
{
    ParametersMapperRecorder *mapper = new ParametersMapperRecorder();

    mapper->setRecorderControlWidget(mRecorderControlWidget);
    mapper->setBaseParametersControlWidget(mBaseControlWidget);
    mapper->setPresentationParametersControlWidget(mPresentationControlWidget);

    connect(mapper, SIGNAL(recorderParamsChanged(QSharedPointer<Recorder>))
            , this, SLOT(recorderControlParametersChanged(QSharedPointer<Recorder>)));

  /*  connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , this, SLOT(baseControlParametersChanged(QSharedPointer<Base>)));*/

    mCalculator = new RecorderThread();

    connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , static_cast<RecorderThread*>(mCalculator)
            , SLOT(baseControlParametersChanged(QSharedPointer<BaseParameters>)));
    connect(mapper, SIGNAL(recorderParamsChanged(QSharedPointer<Recorder>))
            , static_cast<RecorderThread*>(mCalculator)
            , SLOT(recorderControlParametersChanged(QSharedPointer<Recorder>)));
    connect(mapper, SIGNAL(presentationParametersParamsChanged(QSharedPointer<PresentationParameters>))
            , static_cast<RecorderThread*>(mCalculator)
            , SLOT(presentationControlParametersChanged(QSharedPointer<PresentationParameters>)));

    mapper->paramsChanged();

    mParamsMapper = mapper;

    connect(this, SIGNAL(recordingTriggered()), mCalculator, SLOT(toggleRecording()), Qt::QueuedConnection);

    connect(mRecorderControlWidget->ui()->recRestartButton, SIGNAL(released()), mCalculator, SLOT(resetRecording()), Qt::QueuedConnection);
    connect(mRecorderControlWidget->ui()->recPauseButton, SIGNAL(released()), mCalculator, SLOT(toggleRecording()), Qt::QueuedConnection);

    connect(mCalculator, SIGNAL(recordingStateChanged(RecorderThread::RecordingState)), this,
            SLOT(recordingStateChanged(RecorderThread::RecordingState)), Qt::QueuedConnection);

    connect(mCalculator, SIGNAL(errorMessage(QString)),
            this,        SLOT  (errorMessage(QString)), Qt::BlockingQueuedConnection);

}


void RecorderDialog::recorderControlParametersChanged(QSharedPointer<Recorder> params)
{
    if (!params)
        return;

    mRecorderControlParams = params;
}

void RecorderDialog::connectFinishedRecalculation()
{
    RecorderThread *calculator = dynamic_cast<RecorderThread *>(mCalculator);

    if (calculator)
    {
        connect(calculator, SIGNAL(processingFinished(AbstractOutputData *)),
                this, SLOT(finishedRecalculation(AbstractOutputData *)), Qt::QueuedConnection);
        play();
    }
}

void RecorderDialog::openPathSelectDialog()
{
    QString dir_path = QFileDialog::getExistingDirectory( this,
                                                          "Select target directory",
                                                          QDir::homePath()
                                                          );

    if (dir_path.length() > 0)
    {
        mRecorderControlWidget->ui()->pathEdit->setText(dir_path);
    }
}

void RecorderDialog::toggleRecording()
{
    emit recordingTriggered();
}

void RecorderDialog::resetRecording()
{
    emit recordingReset();
}

void RecorderDialog::recordingStateChanged(RecorderThread::RecordingState state)
{
    switch (state)
    {
        case RecorderThread::StateRecordingActive:
        {
            mIsRecording = true;
            mBaseControlWidget->setEnabled(false);
            mRecorderControlWidget->ui()->recStartButton->setEnabled(false);
            mRecorderControlWidget->ui()->recPauseButton->setEnabled(true);
            mRecorderControlWidget->ui()->recRestartButton->setEnabled(true);
            mRecorderControlWidget->ui()->pathEdit->setEnabled(false);
            mRecorderControlWidget->ui()->fileTemplateEdit->setEnabled(false);
            break;
        }
        case RecorderThread::StateRecordingPaused:
        {
            break;
        }
        case RecorderThread::StateRecordingReset:
        case RecorderThread::StateRecordingFailure:
        {
            mIsRecording = false;
            mBaseControlWidget->setEnabled(true);
            mRecorderControlWidget->ui()->recStartButton->setChecked(false);
            mRecorderControlWidget->ui()->recStartButton->setEnabled(true);
            mRecorderControlWidget->ui()->recPauseButton->setChecked(false);
            mRecorderControlWidget->ui()->recPauseButton->setEnabled(false);
            mRecorderControlWidget->ui()->recRestartButton->setEnabled(false);
            mRecorderControlWidget->ui()->pathEdit->setEnabled(true);
            mRecorderControlWidget->ui()->fileTemplateEdit->setEnabled(true);
            break;
        }
    }
}

void RecorderDialog::processResult()
{

    for (unsigned i = 0; i < eventList.size(); i++)
    {
        RecorderOutputData *fod = dynamic_cast<RecorderOutputData*>(eventList[i]);
        if (fod == NULL)
            break;

        mStatsDialog.addStats(fod->stats);

        if (mIsRecording)
            mRecorderControlWidget->ui()->frameCountLabel->setText(QString("Frame (frame pairs) written: %1").arg(fod->frameCount));


//        fod->mMainImage.print();

        if (i == eventList.size() - 1) {
            mImage = QSharedPointer<QImage>(new QImage(fod->mMainImage.width(), fod->mMainImage.height(),  QImage::Format_RGB32));
            fod->mMainImage.drawImage(mImage.data());
        }

        delete fod;
    }
    updateWidgets();
}

void RecorderDialog::errorMessage(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}
