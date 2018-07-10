/**
 * \file copterDialog.cpp
 * \brief Implements frame recording dialog based on BaseHostDialog
 *
 * \date Sep 17, 2010
 * \author Sergey Levi
 */

#include "copterDialog.h"

#include <stdio.h>
#include <QLayout>
#include <QDir>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include "parametersMapper/parametersMapperCopter.h"

CopterDialog::CopterDialog()
    : BaseHostDialog(),
      mIsRecording(false),
      mCopterControlWidget(NULL)
{
//    this->resize(this->width(), this->height() - 65);

}

CopterDialog::~CopterDialog()
{
    terminateCalculator();

    cleanupEventQueue();
}

void CopterDialog::initParameterWidgets()
{
    BaseHostDialog::initParameterWidgets();

    mCopterControlWidget = new CopterControlWidget(this, true, UI_NAME_COPTER);
    dockWidget()->layout()->addWidget(mCopterControlWidget);
    mSaveableWidgets.push_back(mCopterControlWidget);
}

void CopterDialog::createCalculator()
{
    ParametersMapperCopter *mapper = new ParametersMapperCopter();

    mapper->setCopterControlWidget(mCopterControlWidget);
    mapper->setBaseParametersControlWidget(mBaseControlWidget);
    mapper->setPresentationParametersControlWidget(mPresentationControlWidget);

    connect(mapper, SIGNAL(copterParamsChanged(QSharedPointer<Copter>))
            , this, SLOT(copterControlParametersChanged(QSharedPointer<Copter>)));

  /*  connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , this, SLOT(baseControlParametersChanged(QSharedPointer<Base>)));*/

    mCalculator = new CopterThread();

    connect(mapper, SIGNAL(baseParametersParamsChanged(QSharedPointer<BaseParameters>))
            , static_cast<CopterThread*>(mCalculator)
            , SLOT(baseControlParametersChanged(QSharedPointer<BaseParameters>)));
    connect(mapper, SIGNAL(copterParamsChanged(QSharedPointer<Copter>))
            , static_cast<CopterThread*>(mCalculator)
            , SLOT(copterControlParametersChanged(QSharedPointer<Copter>)));
    connect(mapper, SIGNAL(presentationParametersParamsChanged(QSharedPointer<PresentationParameters>))
            , static_cast<CopterThread*>(mCalculator)
            , SLOT(presentationControlParametersChanged(QSharedPointer<PresentationParameters>)));

    mapper->paramsChanged();

    mParamsMapper = mapper;

    connect(this, SIGNAL(recordingTriggered()), mCalculator, SLOT(toggleRecording()), Qt::QueuedConnection);

    connect(mCalculator, SIGNAL(recordingStateChanged(CopterThread::RecordingState)), this,
            SLOT(recordingStateChanged(CopterThread::RecordingState)), Qt::QueuedConnection);

    connect(mCalculator, SIGNAL(errorMessage(QString)),
            this,        SLOT  (errorMessage(QString)), Qt::BlockingQueuedConnection);

}


void CopterDialog::copterControlParametersChanged(QSharedPointer<Copter> params)
{
    if (!params)
        return;

    mCopterControlParams = params;
}

void CopterDialog::connectFinishedRecalculation()
{
    CopterThread *calculator = dynamic_cast<CopterThread *>(mCalculator);

    if (calculator)
    {
        connect(calculator, SIGNAL(processingFinished(AbstractOutputData *)),
                this, SLOT(finishedRecalculation(AbstractOutputData *)), Qt::QueuedConnection);
        play();
    }
}


void CopterDialog::processResult()
{

    for (unsigned i = 0; i < eventList.size(); i++)
    {
        CopterOutputData *fod = dynamic_cast<CopterOutputData*>(eventList[i]);
        if (fod == NULL)
            break;

        mStatsDialog.addStats(fod->stats);

     //        fod->mMainImage.print();

        if (i == eventList.size() - 1) {
            mImage = QSharedPointer<QImage>(new QImage(fod->mMainImage.width(), fod->mMainImage.height(),  QImage::Format_RGB32));
            fod->mMainImage.drawImage(mImage.data());

            Affine3DQ copterPos;
            copterPos = fod->position;//Affine3DQ::Identity();

            Mesh3DDecorated *mesh = new Mesh3DDecorated;
            mesh->switchColor();
            mesh->mulTransform(copterPos);

            mesh->setColor(RGBColor::Green());
            mesh->addIcoSphere(Vector3dd( 5, 5, 0), 2, 2);
            mesh->addIcoSphere(Vector3dd(-5, 5, 0), 2, 2);

            mesh->setColor(RGBColor::Red());
            mesh->addIcoSphere(Vector3dd( 5, -5, 0), 2, 2);
            mesh->addIcoSphere(Vector3dd(-5, -5, 0), 2, 2);
            mesh->popTransform();

            Mesh3DScene *scene = new Mesh3DScene;
            scene->switchColor(true);
            scene->add(*mesh, true);

            delete mesh;

            //scene->prepareMesh(cloud);
            //cloud->addSubObject(, QSharedPointer<Scene3D>(shaded));

            cloud->setNewScenePointer(QSharedPointer<Scene3D>(scene));

        }

        delete fod;
    }
    updateWidgets();
}

void CopterDialog::errorMessage(QString message)
{
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

void CopterDialog::showHistogram()
{
}

void CopterDialog::show3DHistogram()
{
}

void CopterDialog::showRectificationDialog()
{
}

void CopterDialog::doRectify()
{
}

void CopterDialog::resetRectification()
{
}
