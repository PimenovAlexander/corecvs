/**
 * \file egomotionDialog.cpp
 * \brief Implements frame recording dialog based on BaseHostDialog
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include "egomotionDialog.h"

#include <stdio.h>
#include <QLayout>
#include <QDir>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include "parametersMapper/parametersMapperEgomotion.h"

EgomotionDialog::EgomotionDialog()
    : BaseHostDialog(),
      mIsRecording(false),
      mEgomotionParametersControlWidget(NULL)
{
//    this->resize(this->width(), this->height() - 65);

}

EgomotionDialog::~EgomotionDialog()
{
    terminateCalculator();

    cleanupEventQueue();
}

void EgomotionDialog::initParameterWidgets()
{
    BaseHostDialog::initParameterWidgets();


    addImage = static_cast<AdvancedImageWidget *>(createAdditionalWindow("Flow", imageWindow, QIcon()));
    graph = static_cast<GraphPlotDialog *>(createAdditionalWindow("Graph view", graphWindow, QIcon()));


    mEgomotionParametersControlWidget = new EgomotionParametersControlWidget(this, true, UI_NAME_RECORDER);
    dockWidget()->layout()->addWidget(mEgomotionParametersControlWidget);
    mSaveableWidgets.push_back(mEgomotionParametersControlWidget);
}

void EgomotionDialog::createCalculator()
{
    ParametersMapperEgomotion *mapper = new ParametersMapperEgomotion();

    mapper->setEgomotionParametersControlWidget(mEgomotionParametersControlWidget);
    mapper->setBaseParametersControlWidget(mBaseControlWidget);
    mapper->setPresentationParametersControlWidget(mPresentationControlWidget);

    connect(mapper, SIGNAL(egomotionParametersParamsChanged(QSharedPointer<EgomotionParameters>))
            , this, SLOT(egomotionControlParametersChanged(QSharedPointer<EgomotionParameters>)));

  /*  connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , this, SLOT(baseControlParametersChanged(QSharedPointer<Base>)));*/

    mCalculator = new EgomotionThread();

    connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , static_cast<EgomotionThread*>(mCalculator)
            , SLOT(baseControlParametersChanged(QSharedPointer<BaseParameters>)));
    connect(mapper, SIGNAL(egomotionParametersParamsChanged(QSharedPointer<EgomotionParameters>))
            , static_cast<EgomotionThread*>(mCalculator)
            , SLOT(egomotionControlParametersChanged(QSharedPointer<EgomotionParameters>)));
    connect(mapper, SIGNAL(presentationParametersParamsChanged(QSharedPointer<PresentationParameters>))
            , static_cast<EgomotionThread*>(mCalculator)
            , SLOT(presentationControlParametersChanged(QSharedPointer<PresentationParameters>)));

    mapper->paramsChanged();

    mParamsMapper = mapper;

    connect(mCalculator, SIGNAL(errorMessage(QString)),
            this,        SLOT  (errorMessage(QString)), Qt::BlockingQueuedConnection);

}


void EgomotionDialog::egomotionControlParametersChanged(QSharedPointer<EgomotionParameters> params)
{
    if (!params)
        return;

    mEgomotionControlParams = params;
}

void EgomotionDialog::connectFinishedRecalculation()
{
    EgomotionThread *calculator = dynamic_cast<EgomotionThread *>(mCalculator);

    if (calculator)
    {
        connect(calculator, SIGNAL(processingFinished(AbstractOutputData *)),
                this, SLOT(finishedRecalculation(AbstractOutputData *)), Qt::QueuedConnection);
        play();
    }
}

void EgomotionDialog::toggleRecording()
{
    emit recordingTriggered();
}

void EgomotionDialog::resetRecording()
{
    emit recordingReset();
}


void EgomotionDialog::processResult()
{

    for (unsigned i = 0; i < eventList.size(); i++)
    {
        EgomotionOutputData *fod = dynamic_cast<EgomotionOutputData*>(eventList[i]);
        if (fod == NULL)
            break;

        mStatsDialog.addStats(fod->stats);

        {
            for (int var = 0 ; var < 4; var++)
            {
                Quaternion rot = Quaternion::FromMatrix(fod->rot[var]);
                double d = rot.getAngle();
                graph->addGraphPoint(var, d);
            }

            graph->addGraphPoint("Pitch", fod->result.pitch());
            graph->addGraphPoint("Yaw", fod->result.yaw());
            graph->addGraphPoint("Roll", fod->result.roll());

        }


//        fod->mMainImage.print();

        if (i == eventList.size() - 1) {
            mImage = QSharedPointer<QImage>(new QImage(fod->mMainImage.width(), fod->mMainImage.height(),  QImage::Format_RGB32));
            fod->mMainImage.drawImage(mImage.data());


            if (fod->debugOutput != NULL) {
                addImage->setImage(QSharedPointer<QImage>(new RGB24Image(fod->debugOutput)));
            }

            graph->update();
        }

        delete fod;
    }
    updateWidgets();
}

void EgomotionDialog::errorMessage(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}
