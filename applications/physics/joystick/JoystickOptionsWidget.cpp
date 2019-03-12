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
    ui->axisLabel   ->setText(QString::number(conf.axisNumber));
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
    mAxisWidgets.clear();
    mButtonWidgets.clear();
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

    for (int i = 0; i < conf.axisNumber; i++)
    {
        QSlider *slider = new QSlider();
        slider->setMinimum(-32767);
        slider->setMaximum( 32767);
        slider->setValue(0);
        slider->setOrientation(Qt::Horizontal);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setTickInterval(2048);
        slider->setEnabled(false);

        mAxisWidgets.push_back(slider);
        layout->addWidget(slider);
    }

    for (int i = 0; i < conf.buttonNumber; i++)
    {
        //QLabel *label = new QLabel("Button");
        QPushButton *button = new QPushButton(QString("Button"));
        button->setCheckable(false);
        mButtonWidgets.push_back(button);
        layout->addWidget(button);
    }


}

void JoystickOptionsWidget::newData(JoystickState state)
{
    //SYNC_PRINT(("JoystickOptionsWidget::newData(JoystickState &state):called"));

    for (size_t i = 0; i < std::min(state.axis.size(), mAxisWidgets.size()); i++)
    {
        mAxisWidgets[i]->setValue(state.axis[i]);
        //SYNC_PRINT(("Setting axis to %d\n", state.axis[i]));
    }

    for (size_t i = 0; i < std::min(state.button.size(), mButtonWidgets.size()); i++)
    {
        mButtonWidgets[i]->setChecked(state.button[i]);
    }

}

void JoystickListener::newJoystickState(JoystickState state)
{
    QMetaObject::invokeMethod( mTarget, "newData", Qt::QueuedConnection,
                               Q_ARG( JoystickState, state ) );

}
