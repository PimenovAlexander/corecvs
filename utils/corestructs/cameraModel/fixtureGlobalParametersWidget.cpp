#include "fixtureGlobalParametersWidget.h"
#include "ui_fixtureGlobalParametersWidget.h"

FixtureGlobalParametersWidget::FixtureGlobalParametersWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FixtureGlobalParametersWidget)
{
    ui->setupUi(this);
    ui->worldToCameraWidget->setPresentationStyle(true);
    ui->worldToCameraWidget->setEnabled(false);

    QObject::connect(ui->relativePathEdit           , SIGNAL(textChanged(QString)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->hasFinalCoordintesCheckBox , SIGNAL(toggled(bool))       , this, SIGNAL(paramsChanged()));

    QObject::connect(ui->replacePathPushButton, SIGNAL(released()), this, SLOT(replacePath()));
    QObject::connect(ui->changePathPushButton , SIGNAL(released()), this, SLOT(changePath()));
}

FixtureGlobalParametersWidget::~FixtureGlobalParametersWidget()
{
    delete_safe(ui);
}

void FixtureGlobalParametersWidget::setData(const FixtureScene *scene)
{
    SYNC_PRINT(("FixtureGlobalParametersWidget::setData(): called\n"));

    ui->relativePathEdit->setText(QString::fromStdString(scene->getImageSearchPath()));
    ui->hasFinalCoordintesCheckBox->setEnabled(scene->coordinateSystemState == FixtureScene::CoordinateSystemState::final);
    ui->worldToCameraWidget->setParameters(scene->worldFrameToCameraFrame);

    cout << "WorldToCameraFrame:" << scene->worldFrameToCameraFrame.rotor << endl;
    cout << "WorldToCameraFrame:" << scene->worldFrameToCameraFrame.rotor.toMatrix() << endl;

    WorldLocationAngles wl = WorldLocationAngles::FromQuaternion(scene->worldFrameToCameraFrame.rotor);
    cout << "WorldToCameraFrame:" << wl << endl;
    cout << "WorldToCameraFrame:" << wl.toQuaternion() << endl;

}

void FixtureGlobalParametersWidget::changePath()
{
   SYNC_PRINT(("FixtureGlobalParametersWidget::changePath()"));
}

void FixtureGlobalParametersWidget::replacePath()
{
    SYNC_PRINT(("FixtureGlobalParametersWidget::replacePath()"));
}
