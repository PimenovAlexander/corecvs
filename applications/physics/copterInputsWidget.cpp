#include "copterInputsWidget.h"
#include "ui_copterInputsWidget.h"

CopterInputsWidget::CopterInputsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CopterInputsWidget)
{
    ui->setupUi(this);

    sliders[0] = ui->Throttle;
    sliders[1] = ui->Roll;
    sliders[2] = ui->Pitch;
    sliders[3] = ui->Yaw;

    sliders[4] = ui->CH5;
    sliders[5] = ui->CH6;
    sliders[6] = ui->CH7;
    sliders[7] = ui->CH8;

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


