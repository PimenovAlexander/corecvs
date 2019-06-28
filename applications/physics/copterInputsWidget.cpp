#include "copterInputsWidget.h"
#include "ui_copterInputsWidget.h"

CopterInputsWidget::CopterInputsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CopterInputsWidget)
{
    ui->setupUi(this);

    labels[0] = ui->throttleLabel;
    labels[1] = ui->rollLabel;
    labels[2] = ui->pitchLabel;
    labels[3] = ui->yawLabel;

    labels[4] = ui->CH5_label;
    labels[5] = ui->CH6_label;
    labels[6] = ui->CH7_label;
    labels[7] = ui->CH8_label;

    sliders[0] = ui->Throttle;
    sliders[1] = ui->Roll;
    sliders[2] = ui->Pitch;
    sliders[3] = ui->Yaw;

    sliders[4] = ui->CH5;
    sliders[5] = ui->CH6;
    sliders[6] = ui->CH7;
    sliders[7] = ui->CH8;

    channelsNames[0] = "Throttle";
    channelsNames[1] = "Roll";
    channelsNames[2] = "Pitch";
    channelsNames[3] = "Yaw";
    channelsNames[4] = "CH5";
    channelsNames[5] = "CH6";
    channelsNames[6] = "CH7";
    channelsNames[7] = "CH8";

    uiSliderUpdated();
    for (int i = 0; i < CopterInputs::CHANNEL_LAST; i++)
    {
        connect(sliders[i], SIGNAL(valueChanged(int)), this, SLOT(uiSliderUpdated()));
    }

}

CopterInputsWidget::~CopterInputsWidget()
{
    delete ui;
}

void CopterInputsWidget::updateState(CopterInputs inputs)
{
    this->blockSignals(true);
    for (int i = 0; i < CopterInputs::CHANNEL_LAST; i++)
    {
        sliders[i]->setValue(inputs.axis[i]);
        QString q = channelsNames[i] + "-" + "\n" + QString::number(inputs.axis[i]);
        labels[i]->setText(q);
    }
    this->blockSignals(false);
}

void CopterInputsWidget::uiSliderUpdated()
{
    CopterInputs inputs;
    for (int i = 0; i < CopterInputs::CHANNEL_LAST; i++)
    {
        inputs.axis[i] = sliders[i]->value();
    }
    emit uiUpdated(inputs);
}


