#include <QFileDialog>

#include "cameraModelParametersControlWidget.h"
#include "ui_cameraModelParametersControlWidget.h"

#include "core/camerafixture/fixtureScene.h"

CameraModelParametersControlWidget::CameraModelParametersControlWidget(QWidget *parent) :
    ParametersControlWidgetBase(parent),
    ui(new Ui::CameraModelParametersControlWidget)
{
    ui->setupUi(this);

    ui->lensDistortionWidget->toggleAdvanced(false);


    QObject::connect(ui->cameraNameEdit      , SIGNAL(textChanged(QString)), this, SLOT(paramsChangedInUI()));
    QObject::connect(ui->lensDistortionWidget, SIGNAL(paramsChanged())     , this, SLOT(paramsChangedInUI()));
    QObject::connect(ui->extrinsicWorldWidget, SIGNAL(paramsChanged())     , this, SLOT(paramsChangedInUI()));

    /*QObject::connect(ui->spinBoxFocalX, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    QObject::connect(ui->spinBoxFocalY, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));

    QObject::connect(ui->spinBoxCx, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    QObject::connect(ui->spinBoxCy, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));

    QObject::connect(ui->spinBoxSizeX, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    QObject::connect(ui->spinBoxSizeY, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));

    QObject::connect(ui->spinBoxSizeDistortedX, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));
    QObject::connect(ui->spinBoxSizeDistortedY, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));

    QObject::connect(ui->spinBoxSkew, SIGNAL(valueChanged(double)), this, SLOT(paramsChangedInUI()));*/

    /* So far just hide old stuff*/
    /*if (true)
    {
        ui->oldFrame->hide();
        ui->spinBoxFocalX->hide();
        ui->spinBoxFocalY->hide();

        ui->spinBoxCx->hide();
        ui->spinBoxCy->hide();

        ui->spinBoxSizeX->hide();
        ui->spinBoxSizeY->hide();

        ui->spinBoxSizeDistortedX->hide();
        ui->spinBoxSizeDistortedY->hide();

        ui->spinBoxSkew->hide();
    }*/


    //QObject::connect(ui->showDistortionCheckBox, SIGNAL(toggled(bool)), ui->lensDistortionWidget, SLOT(setVisible(bool)));
    // QObject::connect(ui->showProjection        , SIGNAL(toggled(bool)), ui->projection, SLOT(setVisible(bool));
    //QObject::connect(ui->showExtrinsicsCheckBox, SIGNAL(toggled(bool)), ui->tabWidget,            SLOT(setVisible(bool)));

//    writeUi();
    /* Addintional buttons */

    QObject::connect(ui->loadPushButton, SIGNAL(released()), this, SLOT(loadPressed()));
    QObject::connect(ui->savePushButton, SIGNAL(released()), this, SLOT(savePressed()));
    QObject::connect(ui->revertButton  , SIGNAL(released()), this, SLOT(revertPressed()));

    /* Extrinsics reset button */
    QObject::connect(ui->zeroExtrinsicsPushButton , SIGNAL(released()), this, SLOT( zeroPressed()));
    QObject::connect(ui->resetExtrinsicsPushButton, SIGNAL(released()), this, SLOT(resetPressed()));

    ui->extrinsicCamWidget->setEnabled(false);

    /* */
    for (int i = 0; i < ProjectionType::PROJECTIONTYPE_LAST; i++)
    {
        ui->projectionTypeComboBox->addItem(ProjectionType::getName((ProjectionType::ProjectionType)i));
    }
    QObject::connect(ui->projectionTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(paramsChangedInUI()));

    QGridLayout *glayout = new QGridLayout;
    glayout->setMargin(2);
    glayout->setContentsMargins(2,2,2,2);
    glayout->setHorizontalSpacing(2);
    glayout->setVerticalSpacing(2);
    ui->projectionFrame->setLayout(glayout);
}

CameraModelParametersControlWidget::~CameraModelParametersControlWidget()
{
    delete_safe(ui);
}

#if 0
LensDistortionModelParameters CameraModelParametersControlWidget::lensDistortionParameters()
{
    LensDistortionModelParameters result;
    ui->lensDistortionWidget->getParameters(result);
    return result;
}

void CameraModelParametersControlWidget::setLensDistortionParameters(const LensDistortionModelParameters &params)
{
    ui->lensDistortionWidget->setParameters(params);
}
#endif

void CameraModelParametersControlWidget::loadParamWidget(WidgetLoader &loader)
{
    CameraModel mCameraModel;
    loader.loadParameters<CameraModel>(mCameraModel, "camera");
    setParameters(mCameraModel);
}


void CameraModelParametersControlWidget::saveParamWidget(WidgetSaver &saver)
{
    CameraModel mCameraModel;
    getParameters(mCameraModel);
    saver.saveParameters<CameraModel>(mCameraModel, "camera");
}

void CameraModelParametersControlWidget::loadPressed()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "LOAD: Choose an camera config name",
        ".",
        "Text (*.json *.txt)"
    );
    if (!filename.isEmpty()) {
        emit loadRequest(filename);
    }
}

void CameraModelParametersControlWidget::savePressed()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "SAVE: Choose an camera config name",
        ".",
        "Text (*.json)"
    );
    if (!filename.isEmpty()) {
        emit saveRequest(filename);
    }
}

void CameraModelParametersControlWidget::revertPressed()
{
//    qDebug() << "CameraModelParametersControlWidget::revertPressed(): pressed";
    setParameters(backup);
}

void CameraModelParametersControlWidget::zeroPressed()
{
    Affine3DQ zero = Affine3DQ::Identity();
    ui->extrinsicWorldWidget->setParameters(zero);
    emit paramsChanged();
}

void CameraModelParametersControlWidget::resetPressed()
{
    Affine3DQ zero = FixtureScene::DEFAULT_WORLD_TO_CAMERA.inverted();
    ui->extrinsicWorldWidget->setParameters(zero);
    emit paramsChanged();
}

void CameraModelParametersControlWidget::assertProjectionMatch()
{
    const Reflection *ref = ProjectionFactory::reflectionById((ProjectionType::ProjectionType)ui->projectionTypeComboBox->currentIndex());
    if (intrinsicsWidget == NULL || ref != intrinsicsWidget->reflection)
    {
        delete_safe(intrinsicsWidget);
        intrinsicsWidget = new ReflectionWidget(ref);
        QObject::connect(intrinsicsWidget, SIGNAL(paramsChanged()), this, SLOT(paramsChangedInUI()));
        ui->projectionFrame->layout()->addWidget(intrinsicsWidget);
     }
}

void CameraModelParametersControlWidget::paramsChangedInUI()
{
    assertProjectionMatch();

//    qDebug() << "CameraModelParametersControlWidget::paramsChangedInUI(): pressed";
    ui->revertButton->setEnabled(true);
    emit paramsChanged();
}

/**/

void CameraModelParametersControlWidget::getParameters(CameraModel& params) const
{    
    ui->lensDistortionWidget->getParameters(params.distortion);

    params.nameId = ui->cameraNameEdit->text().toStdString();

    Affine3DQ location;
    ui->extrinsicWorldWidget->getParameters(location);
    params.setLocation(location);

    ProjectionType::ProjectionType curId = (ProjectionType::ProjectionType) ui->projectionTypeComboBox->currentIndex();
    if (params.intrinsics->projection != curId)
    {
        params.intrinsics.reset(ProjectionFactory::projectionById(curId));
    }

    const Reflection *ref = ProjectionFactory::reflectionById(params.intrinsics->projection);
    if (intrinsicsWidget != NULL && intrinsicsWidget->reflection == ref)
    {
        DynamicObjectWrapper wrapper = params.intrinsics->getDynamicWrapper();
        intrinsicsWidget->getParameters(wrapper.rawObject);
    }


}

CameraModel *CameraModelParametersControlWidget::createParameters() const
{

    /**
     * We should think of returning parameters by value or saving them in a preallocated place
     **/
    CameraModel *result = new CameraModel();
    getParameters(*result);
    return result;
}

void CameraModelParametersControlWidget::setParameters(const CameraModel &input)
{
    SYNC_PRINT(("CameraModelParametersControlWidget::setParameters()\n"));
    // Block signals to send them all at once
    bool wasBlocked = blockSignals(true);

    ui->cameraNameEdit->setText(QString::fromStdString(input.nameId));
    ui->lensDistortionWidget->setParameters(input.distortion);

    ui->extrinsicWorldWidget->setParameters(input.getAffine());
    ui->extrinsicCamWidget->setParameters(FixtureScene::DEFAULT_WORLD_TO_CAMERA * input.getAffine());

#if 0
    PinholeCameraIntrinsics *pinhole = input.getPinhole();

    if (pinhole != NULL)
    {
        ui->spinBoxFocalX->setValue(pinhole->focalX());
        ui->spinBoxFocalY->setValue(pinhole->focalY());

        ui->spinBoxCx->setValue(pinhole->principalX());
        ui->spinBoxCy->setValue(pinhole->principalY());

        ui->spinBoxSizeX->setValue(pinhole->sizeX());
        ui->spinBoxSizeX->setValue(pinhole->sizeY());

        ui->spinBoxSizeDistortedX->setValue(pinhole->distortedSizeX());
        ui->spinBoxSizeDistortedY->setValue(pinhole->distortedSizeY());

        ui->spinBoxSkew->setValue(pinhole->skew());

        /*ui->infoLabel->setText(QString("Size(xy):[%1 x %2] dist:[%3 x %4]")
                    .arg(pinhole->sizeX())         .arg(pinhole->sizeY())
                    .arg(pinhole->distortedSizeX()).arg(pinhole->distortedSizeY()));*/
    }
#endif

    /**/
    ui->projectionTypeComboBox->setCurrentIndex(input.intrinsics->projection);

    assertProjectionMatch();
    DynamicObjectWrapper wrapper = input.intrinsics->getDynamicWrapper();
    intrinsicsWidget->setParameters(wrapper.rawObject);

    blockSignals(wasBlocked);
    backup = input;
    ui->revertButton->setEnabled(false);

    emit paramsChanged();
}

void CameraModelParametersControlWidget::setParametersVirtual(void *input)
{
    // Modify widget parameters from outside
    CameraModel *inputCasted = static_cast<CameraModel *>(input);
    setParameters(*inputCasted);
}



#if 0
FixtureCameraParametersControlWidget::FixtureCameraParametersControlWidget(QWidget *parent) :
    CameraModelParametersControlWidget(parent)
{
    QComboBox *prototypeBox = new QComboBox();
    prototypeBox->addItem("NULL");
    prototypeBox->addItem("Dummy");

    ui->mainLayout->addWidget(prototypeBox, 0, 0, 1, 1);
}
#endif
