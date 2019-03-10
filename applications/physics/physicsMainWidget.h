#ifndef PHYSICSMAINWIDGET_H
#define PHYSICSMAINWIDGET_H

#include <stack>
#include <linux/joystick.h>
#include <fcntl.h>

#include <JoystickOptionsWidget.h>
#include <QWidget>
#include <aboutDialog.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "clientSender.h"
#include "qComController.h"


#include "controlRecord.h"
#include "joystickInput.h"
#include "simulation.h"
#include "core/geometry/mesh3DDecorated.h"
#include "mesh3DScene.h"

namespace Ui {
    class PhysicsMainWidget;
}

class PhysicsMainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PhysicsMainWidget(QWidget *parent = 0);
    QSerialPort serialPort;

    QByteArray *flyCommandWriteData = NULL;
    QByteArray getDataFromSticks();

    int currentMode=0;
    bool realModeActive = false;
    bool virtualModeActive = false;

    struct AxisState {
        short x;
        short y;
    };

    void disconnectFromCopter();

    virtual ~PhysicsMainWidget();


/** **/
public:
    JoystickOptionsWidget mJoystickSettings;
    Mesh3DScene *mesh;
    /** We have to use it to satisfy the icons license **/
    AboutDialog mAboutWidget;

public slots:
    void settingsWidget();
    void aboutWidget();
    void keepAlive();
private slots:



    void startVirtualMode();
    void yawChange(int i);
    void rollChange(int i);
    void pitchChange(int i);
    void throttleChange(int i);
    void CH5Change(int i);
    void CH6Change(int i);
    void CH7Change(int i);
    void CH8Change(int i);
    void startJoyStickMode();


    void onStartVirtualModeReleased();

    //void on_pushButton_released();

    void sendJoyValues();
    void startRealMode();
    // void BindToRealDrone();
    void stop();
    void onPushButton2Clicked();


    void on_comboBox_currentTextChanged(const QString &arg1);

private:
    struct Message {
        int throttle;
        int roll;
        int yaw;
        int pitch ;
        int countOfRepeats;
    };

    std::list<Message> messages;


    JoyStickInput joystick1 {
        yawValue,
        rollValue,
        pitchValue,
        throttleValue,
                CH5Value, CH6Value, CH7Value, CH8Value};

    QComController ComController {this, yawValue, rollValue, pitchValue, throttleValue,
                CH5Value, CH6Value, CH7Value, CH8Value};

    Ui::PhysicsMainWidget *ui;
    int yawValue;
    int rollValue;
    int pitchValue;
    int throttleValue;
    int throttleValueFromJS;
    int CH5Value;
    int CH6Value;
    int CH7Value;
    int CH8Value;
    int midThrottle = 1350;

    int counter;
    int sign(int val);
    clock_t startTime;
    void showValues( );
    void frameValuesUpdate();
    bool created=false;
    bool bind=false;
    bool arming=false;
    bool startFly=false;   //to set mid throttle after arming

    bool rtPressed=false;
    bool ltPressed=false;

    bool recording=false;

    void sendOurValues(std::vector<uint8_t> OurValues);
    bool virtuaModeActive=false;

    ControlRecord recordData;

    Simulation simSim;

    struct AxisState axes[3];

    ClientSender VirtualSender;

    int countOfSticks=0;

    bool autopilotMode=false;
    stack<Message> autopilotStack;
};

#endif // PHYSICSMAINWIDGET_H
