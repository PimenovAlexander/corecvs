#include "radio/multimoduleController.h"
#include "radioControlWidget.h"
#include "ui_radioControlWidget.h"

using namespace std;

RadioControlWidget::RadioControlWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RadioControlWidget)
{
    ui->setupUi(this);    
    rereadDevices();
    ui->modelIDComboBox->setCurrentIndex(9);
}

RadioControlWidget::~RadioControlWidget()
{
    delete ui;
}


void RadioControlWidget::rereadDevices()
{
    ui->comboBox->clear();
    vector<string> devices = MultimoduleController::getDevices();

    for (size_t i = 0; i < devices.size(); i ++)
    {
        ui->comboBox->addItem(QString::fromStdString(devices[i]));
    }
}

void RadioControlWidget::connect()
{
    controller.protocol.lowPower = ui->lowPowerCheckBox->isChecked();
    int subtype = ui->protocolSubtypeComboBox->currentIndex();
    controller.protocol.subtype  = subtype;
    controller.protocol.modelId  = ui->modelIDComboBox->currentIndex();
    if (controller.connectToModule(ui->deviceLineEdit->text().toStdString()))
    {
        ui->connectPushButton   ->setEnabled(false);
        ui->disconnectPushButton->setEnabled(true);
    }
}

void RadioControlWidget::disconnect()
{
    if (controller.disconnect()) {
        ui->connectPushButton   ->setEnabled(true);
        ui->disconnectPushButton->setEnabled(false);
    }

}
