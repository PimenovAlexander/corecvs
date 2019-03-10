#include "mixerChannelOperation.h"
#include "ui_mixerChannelOperation.h"

MixerChannelOperation::MixerChannelOperation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MixerChannelOperation)
{
    ui->setupUi(this);
}

MixerChannelOperation::~MixerChannelOperation()
{
    delete ui;
}
