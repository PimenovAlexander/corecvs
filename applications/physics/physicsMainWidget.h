#ifndef PHYSICSMAINWIDGET_H
#define PHYSICSMAINWIDGET_H

#include <QWidget>
#include "clientsender.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <linux/joystick.h>

namespace Ui {
class PhysicsMainWidget;
}

class PhysicsMainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PhysicsMainWidget(QWidget *parent = 0);
    QSerialPort serialPort;
    ~PhysicsMainWidget();
     QByteArray *FlyCommandWriteData;
     QByteArray GetDataFromStics();

     bool RealModeActive = false;
     bool VirtualModeActive = false;
public slots:
    void keepAlive();
private slots:
    void yawChange(int i);
    void rollChange(int i);
    void pitchChange(int i);
    void throttleChange(int i);
    void StartJoyStickMode();
    void Bind();
    void on_pushButton_released();
    void SendJoyValues();
    void StartRealMode();
    void BindToRealDrone();
    void STOP();
    void on_pushButton_2_clicked();

private:
    Ui::PhysicsMainWidget *ui;
    int yaw_value;
    int roll_value;
    int pitch_value;
    int throttle_value;
    int throttle_value_from_JS;
    int fifth_CH;
    int sixth_CH;
    int seventh_CH;
    int eighth_CH;
    int counter;
    int sign(int val);
    clock_t start_time;
    void ShowValues( );
    void FrameValuesUpdate();
    bool created=false;
    bool bind=false;
    bool Arming=false;
    bool startFly=false;   //to set mid throttle after arming
    void SendOurValues(std::vector<uint8_t> OurValues);

    ClientSender VirtualSender;
};

#endif // PHYSICSMAINWIDGET_H
