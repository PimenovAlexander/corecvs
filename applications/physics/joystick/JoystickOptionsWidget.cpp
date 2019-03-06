#include "JoystickOptionsWidget.h"
#include "ui_JoystickOptionsWidget.h"

#include "joystickInterface.h"

//using namespace corecvs;
using namespace std;


JoystickOptionsWidget::JoystickOptionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoystickOptionsWidget)
{
    ui->setupUi(this);
    rereadDevices();
}

JoystickOptionsWidget::~JoystickOptionsWidget()
{
    delete ui;
}

void JoystickOptionsWidget::rereadDevices()
{
    ui->comboBox->clear();
    vector<string> devices = JoystickInterface::getDevices();

    for (size_t i = 0; i < devices.size(); i ++)
    {
        ui->comboBox->addItem(QString::fromStdString(devices[i]));
    }
}

void JoystickOptionsWidget::getProps()
{
    JoystickConfiguration conf = JoystickInterface::getConfiguration(ui->deviceLineEdit->text().toStdString());
    conf.print();

    ui->nameLabel   ->setText(QString::fromStdString(conf.name));
    ui->axisLabel   ->setText(QString::number(conf.stickNumber));
    ui->buttonsLabel->setText(QString::number(conf.buttonNumber));
    ui->versionLabel->setText(QString::number(conf.version));
}
