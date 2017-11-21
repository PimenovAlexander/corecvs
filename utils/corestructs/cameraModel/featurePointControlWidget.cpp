#include "featurePointControlWidget.h"
#include "ui_featurePointControlWidget.h"

FeaturePointControlWidget::FeaturePointControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FeaturePointControlWidget)
{
    ui->setupUi(this);

    QObject::connect(ui->nameLineEdit,           SIGNAL(textChanged(QString)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->xSpinBox,               SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->ySpinBox,               SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->zSpinBox,               SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->hasPositionCheckBox,    SIGNAL(toggled(bool)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->xReprSpinBox,           SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->yReprSpinBox,           SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->zReprSpinBox,           SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->hasReprojectedCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->color,                  SIGNAL(paramsChanged()), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->posToReprojButton,      SIGNAL(released()), this, SLOT(posToReproj()));
    QObject::connect(ui->reprojToPosButton,      SIGNAL(released()), this, SLOT(reprojToPos()));
}

FeaturePointControlWidget::~FeaturePointControlWidget()
{
    delete ui;
}

void FeaturePointControlWidget::getParameters(SceneFeaturePoint &params) const
{
    params.name = ui->nameLineEdit->text().toStdString();

    params.position.x() = ui->xSpinBox->value();
    params.position.y() = ui->ySpinBox->value();
    params.position.z() = ui->zSpinBox->value();

    params.hasKnownPosition = ui->hasPositionCheckBox->isChecked();

    params.reprojectedPosition.x() = ui->xReprSpinBox->value();
    params.reprojectedPosition.y() = ui->yReprSpinBox->value();
    params.reprojectedPosition.z() = ui->zReprSpinBox->value();

    params.hasKnownReprojectedPosition = ui->hasReprojectedCheckBox->isChecked();

    params.color = ui->color->getColor();
}

void FeaturePointControlWidget::setParameters(const SceneFeaturePoint &input)
{
    // Block signals to send them all at once
    bool wasBlocked = blockSignals(true);

    ui->nameLineEdit->setText(QString::fromStdString(input.name));

    ui->xSpinBox->setValue(input.position.x());
    ui->ySpinBox->setValue(input.position.y());
    ui->zSpinBox->setValue(input.position.z());

    ui->hasPositionCheckBox->setChecked(input.hasKnownPosition);

    ui->xReprSpinBox->setValue(input.reprojectedPosition.x());
    ui->yReprSpinBox->setValue(input.reprojectedPosition.y());
    ui->zReprSpinBox->setValue(input.reprojectedPosition.z());

    ui->hasReprojectedCheckBox->setChecked(input.hasKnownReprojectedPosition);

    ui->color->setRGBColor(input.color);

    QString str = QString("Observations: %1").arg(input.observations.size());
    ui->errLabel->setText(str);

    blockSignals(wasBlocked);
    emit paramsChanged();
}

void FeaturePointControlWidget::setParametersVirtual(void *input)
{
    // Modify widget parameters from outside
    SceneFeaturePoint *inputCasted = static_cast<SceneFeaturePoint *>(input);
    setParameters(*inputCasted);
}

void FeaturePointControlWidget::setEnabled(bool flag)
{
    //qDebug("FeaturePointControlWidget::setEnabled(flag = %s)", flag ? "true" : "false");

    ui->nameLineEdit->setEnabled(flag);

    ui->xSpinBox->setEnabled(flag);
    ui->ySpinBox->setEnabled(flag);
    ui->zSpinBox->setEnabled(flag);

    ui->hasPositionCheckBox->setEnabled(flag);

    ui->xReprSpinBox->setEnabled(flag);
    ui->yReprSpinBox->setEnabled(flag);
    ui->zReprSpinBox->setEnabled(flag);

    ui->hasReprojectedCheckBox->setEnabled(flag);

    ui->color->setEnabled(flag);
}

void FeaturePointControlWidget::posToReproj()
{
    bool wasBlocked = blockSignals(true);

    ui->xReprSpinBox->setValue(ui->xSpinBox->value());
    ui->yReprSpinBox->setValue(ui->ySpinBox->value());
    ui->zReprSpinBox->setValue(ui->zSpinBox->value());

    blockSignals(wasBlocked);
    emit paramsChanged();
}

void FeaturePointControlWidget::reprojToPos()
{
    bool wasBlocked = blockSignals(true);

    ui->xSpinBox->setValue(ui->xReprSpinBox->value());
    ui->ySpinBox->setValue(ui->yReprSpinBox->value());
    ui->zSpinBox->setValue(ui->zReprSpinBox->value());

    blockSignals(wasBlocked);
    emit paramsChanged();
}
