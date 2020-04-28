#include "JoystickOptionsWidget.h"
#include "ui_JoystickOptionsWidget.h"

#include "linuxJoystickInterface.h"
#include "core/utils/global.h"

using namespace std;

using namespace corecvs;

JoystickOptionsWidget::JoystickOptionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JoystickOptionsWidget)
{
    ui->setupUi(this);
    rereadDevices();

    GraphPlotParameters graphParams = ui->graphWidget->getParameters();
    graphParams.setYScale(35000);
    ui->graphWidget->setParameters(graphParams);
}

JoystickOptionsWidget::~JoystickOptionsWidget()
{
    delete ui;
}

void JoystickOptionsWidget::rereadDevices()
{
    ui->comboBox->clear();
    vector<string> devices = LinuxJoystickInterface::getDevices();

    for (size_t i = 0; i < devices.size(); i ++)
    {
        ui->comboBox->addItem(QString::fromStdString(devices[i]));
    }

    devices = PlaybackJoystickInterface::getDevices();
    for (size_t i = 0; i < devices.size(); i ++)
    {
        ui->comboBox->addItem(QString::fromStdString(devices[i]));
    }

}

void JoystickOptionsWidget::getProps()
{
    JoystickConfiguration conf = LinuxJoystickInterface::getConfiguration(ui->deviceLineEdit->text().toStdString());
    conf.print();

    ui->nameLabel   ->setText(QString::fromStdString(conf.name));
    ui->axisLabel   ->setText(QString::number(conf.axisNumber));
    ui->buttonsLabel->setText(QString::number(conf.buttonNumber));
    ui->versionLabel->setText(QString::number(conf.version));
}

void JoystickOptionsWidget::openJoystick()
{
    SYNC_PRINT(("JoystickOptionsWidget::openJoystick() : called\n"));
    if (mInterface != NULL) {
        return;
    }

    std::string name  = ui->deviceLineEdit->text().toStdString();
    SYNC_PRINT(("JoystickOptionsWidget::openJoystick() : creating joystick for <%s>\n", name.c_str()));

    if (HelperUtils::endsWith(name, ".dump")) {
        SYNC_PRINT(("Created PlaybackJoystickInterface\n"));
        mInterface = new JoystickListener<PlaybackJoystickInterface>(name, this);
    } else {
        SYNC_PRINT(("Created LinuxJoystickInterface\n"));
        mInterface = new JoystickListener<LinuxJoystickInterface>(name, this);
    }

    mInterface->start();

    JoystickConfiguration conf = mInterface->getConfiguration();
    conf.print();
    reconfigure(conf);
    QObject::connect(mInterface, SIGNAL(joystickUpdated(JoystickState)), this, SLOT(newData(JoystickState)), Qt::QueuedConnection);
    QObject::connect(mInterface, SIGNAL(joystickUpdated(JoystickState)), this, SIGNAL(joystickUpdated(JoystickState)), Qt::QueuedConnection);

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

void JoystickOptionsWidget::recordJoystick()
{
    recording = !recording;

    if (!recording)
    {
        record.save("joystick.dump");
    } else {
        record.reset(currentConfiguation);
    }

    if (recording) {
        ui->recordPushButton->setText("Stop Record");
    } else {
        ui->recordPushButton->setText("Record");
    }
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
    currentConfiguation = conf;
    clearDialog();

    QLayout *layout = ui->mappingBox->layout();

    for (int i = 0; i < conf.axisNumber; i++)
    {
        /*QSlider *slider = new QSlider();
        slider->setMinimum(-32767);
        slider->setMaximum( 32767);
        slider->setValue(0);
        slider->setOrientation(Qt::Horizontal);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setTickInterval(2048);
        slider->setEnabled(false);*/

        MixerChannelOperationWidget *widget = new MixerChannelOperationWidget();
        mAxisWidgets.push_back(widget);
        layout->addWidget(widget);
    }

    for (int i = 0; i < conf.buttonNumber; i++)
    {
        //QLabel *label = new QLabel("Button");
        QPushButton *button = new QPushButton(QString("Button"));
        button->setEnabled(false);
        button->setCheckable(true);
        mButtonWidgets.push_back(button);
        layout->addWidget(button);
    }


}

void JoystickOptionsWidget::newData(JoystickState state)
{
    //SYNC_PRINT(("JoystickOptionsWidget::newData(JoystickState &state):called"));

    for (size_t i = 0; i < std::min(state.axis.size(), mAxisWidgets.size()); i++)
    {
        mAxisWidgets[i]->setInput(state.axis[i]);
        //SYNC_PRINT(("Setting axis to %d\n", state.axis[i]));
    }

    for (size_t i = 0; i < std::min(state.button.size(), mButtonWidgets.size()); i++)
    {
        mButtonWidgets[i]->setChecked(state.button[i]);
        // SYNC_PRINT(("Setting button to %d\n", state.button[i]));
    }

    if (recording) {
        record.addState(state);
    }


    for (size_t i = 0; i < state.axis.size(); i++)
    {
        ui->graphWidget->addGraphPoint(i, state.axis[i]);
    }

    for (size_t i = 0; i < state.button.size(); i++)
    {
        ui->graphWidget->addGraphPoint(i + state.axis.size(), state.button[i]);
    }
    ui->graphWidget->update();
}


