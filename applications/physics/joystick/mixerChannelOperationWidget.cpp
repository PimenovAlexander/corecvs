#include "mixerChannelOperationWidget.h"
#include "ui_mixerChannelOperationWidget.h"

MixerChannelOperationWidget::MixerChannelOperationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MixerChannelOperationWidget)
{
    ui->setupUi(this);
}

MixerChannelOperationWidget::~MixerChannelOperationWidget()
{
    delete ui;
}
