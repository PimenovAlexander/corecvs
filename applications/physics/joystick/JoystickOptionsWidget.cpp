#include "JoystickOptionsWidget.h"
#include "ui_JoystickOptionsWidget.h"

#include "joystickInterface.h"
#include "core/utils/global.h"

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

void JoystickOptionsWidget::openJoystick()
{
    if (mInterface != NULL) {
        return;
    }
    mInterface = new JoystickListener(ui->deviceLineEdit->text().toStdString(), this);

    JoystickConfiguration conf = JoystickListener::getConfiguration(mInterface->mDeviceName.c_str());
    conf.print();
    reconfigure(conf);
    mInterface->start();
    ui-> openPushButton->setEnabled(false);
    ui->closePushButton->setEnabled(true);
}

void JoystickOptionsWidget::closeJoystick()
{
    if (mInterface == NULL)
    {
        return;
    }

    mInterface->stop();
    delete_safe(mInterface);

    ui-> openPushButton->setEnabled(true);
    ui->closePushButton->setEnabled(false);
}

void JoystickOptionsWidget::clearDialog()
{
    QLayout *layout = ui->mappingBox->layout();
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }
}

void JoystickOptionsWidget::reconfigure(JoystickConfiguration &conf)
{
    clearDialog();

    QLayout *layout = ui->mappingBox->layout();

    for (int i = 0; i < conf.stickNumber; i++)
    {
        QLabel *label = new QLabel("Axis");
        layout->addWidget(label);
    }

    for (int i = 0; i < conf.buttonNumber; i++)
    {
        QLabel *label = new QLabel("Button");
        layout->addWidget(label);
    }


}

void JoystickOptionsWidget::newData(JoystickState state)
{
    //SYNC_PRINT(("JoystickOptionsWidget::newData(JoystickState &state):called"));

    clearDialog();

    QLayout *layout = ui->mappingBox->layout();

    for (size_t i = 0; i < state.axis.size(); i++)
    {
        QLabel *label = new QLabel(QString("Axis %1").arg(state.axis[i]));
        layout->addWidget(label);
    }

    for (size_t i = 0; i < state.button.size(); i++)
    {
        QLabel *label = new QLabel(QString("Button %1").arg(state.button[i]));
        layout->addWidget(label);
    }

}

void JoystickListener::newJoystickState(JoystickState state)
{
    QMetaObject::invokeMethod( mTarget, "newData", Qt::QueuedConnection,
                               Q_ARG( JoystickState, state ) );

}
