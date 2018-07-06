#include "fixtureCameraControlWidget.h"
#include "ui_fixtureCameraControlWidget.h"

#include "core/camerafixture/fixtureScene.h"

FixtureCameraControlWidget::FixtureCameraControlWidget(QWidget *parent) :
    ParametersControlWidgetBase(parent),
    ui(new Ui::FixtureCameraControlWidget)
{
    ui->setupUi(this);
    ui->prototypesComboBox->addItem("NULL");

    QObject::connect(ui->cameraModelWidget, SIGNAL(loadRequest(QString)), this, SIGNAL(loadRequest(QString)));
    QObject::connect(ui->cameraModelWidget, SIGNAL(saveRequest(QString)), this, SIGNAL(saveRequest(QString)));

    QObject::connect(ui->cameraModelWidget, SIGNAL( valueChanged()), this, SIGNAL( valueChanged()));
    QObject::connect(ui->cameraModelWidget, SIGNAL(paramsChanged()), this, SIGNAL(paramsChanged()));
}

FixtureCameraControlWidget::~FixtureCameraControlWidget()
{
    delete ui;
}

void FixtureCameraControlWidget::getParameters(FixtureCamera &params) const
{
    ui->cameraModelWidget->getParameters(params);
}

void FixtureCameraControlWidget::setParameters(const FixtureCamera &input)
{
    SYNC_PRINT(("FixtureCameraControlWidget::setParameters()\n"));

    const FixtureCamera *camera = static_cast<const FixtureCamera *>(&input);
    FixtureScene *scene = camera->ownerScene;
    if (scene != NULL) {
        while (ui->prototypesComboBox->count() != 1)
        {
            ui->prototypesComboBox->removeItem(ui->prototypesComboBox->count() - 1);
        }

        for (CameraPrototype *proto : scene->cameraPrototypes())
        {
            if (proto == NULL)
                continue;
            ui->prototypesComboBox->addItem(QString::fromStdString(proto->nameId));
        }
    }

    ui->cameraModelWidget->setParameters(input);
}

void FixtureCameraControlWidget::setParametersVirtual(void *input)
{
    FixtureCamera *camera = static_cast<FixtureCamera *>(input);
    ui->cameraModelWidget->setParametersVirtual(static_cast<CameraModel *>(camera));
}

void FixtureCameraControlWidget::loadParamWidget(WidgetLoader &loader)
{
    ui->cameraModelWidget->loadParamWidget(loader);
}

void FixtureCameraControlWidget::saveParamWidget(WidgetSaver &saver)
{
    ui->cameraModelWidget->saveParamWidget(saver);
}
