#include "mixerChannelOperationWidget.h"
#include "ui_mixerChannelOperationWidget.h"

MixerChannelOperationWidget::MixerChannelOperationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MixerChannelOperationWidget)
{
    ui->setupUi(this);

    ui->inputSlider->setMinimum(-32767);
    ui->inputSlider->setMaximum( 32767);
    ui->inputSlider->setValue(0);
    ui->inputSlider->setOrientation(Qt::Horizontal);
    ui->inputSlider->setTickPosition(QSlider::TicksBelow);
    ui->inputSlider->setTickInterval(2048*4);
    ui->inputSlider->setEnabled(false);
}

MixerChannelOperationWidget::~MixerChannelOperationWidget()
{
    delete ui;
}

void MixerChannelOperationWidget::setInput(int input)
{
    ui->inputSlider->setValue(input);
    ui->inuputLabel->setText(QString::number(input));
}

void MixerChannelOperationWidget::setOutput(int output)
{

}
