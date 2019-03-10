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



const char *CopterInputs::getName(CopterInputs::ChannelID value)
{
    switch (value)
    {
    case CHANNEL_YAW : return "CHANNEL_YAW"; break ;
    case CHANNEL_ROLL : return "CHANNEL_ROLL"; break ;
    case CHANNEL_PITCH : return "CHANNEL_PITCH"; break ;
    case CHANNEL_THROTTLE : return "CHANNEL_THROTTLE"; break ;
    case CHANNEL_4 : return "CHANNEL_4"; break ;
    case CHANNEL_5 : return "CHANNEL_5"; break ;
    case CHANNEL_6 : return "CHANNEL_6"; break ;
    case CHANNEL_7 : return "CHANNEL_7"; break ;
    default : return "Not in range"; break ;

    }
    return "Not in range";
}

CopterInputs::CopterInputs()
{
    axis[CHANNEL_YAW]      = 1500;
    axis[CHANNEL_ROLL]     = 1500;
    axis[CHANNEL_PITCH]    = 1500;
    axis[CHANNEL_THROTTLE] = 1100;

    axis[CHANNEL_4] = 900;
    axis[CHANNEL_5] = 1500;
    axis[CHANNEL_6] = 1500;
    axis[CHANNEL_7] = 1500;
}
