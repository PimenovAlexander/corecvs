#ifndef QCOMCONTROLLER_H
#define QCOMCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include "controllrecord.h"
static  int v=1500;
static  int throt_v=1100;


class QComController : public QObject
{
    Q_OBJECT
public:
    explicit QComController(QObject *parent = nullptr,int & _yaw_value = v, int &_roll_value= v, int &_pitch_value= v, int &_throttle_value= throt_v, int &_CH5_value= v, int &_CH6_value= v, int &_CH7_value= v,  int &_CH8_value= v );
         void BindToRealDrone();
        void SendOurValues(std::vector<uint8_t> OurValues);
  int ppp;
        int& yaw_value;
        int& roll_value;
        int& pitch_value;
        int& throttle_value;
        int& CH5_value;
        int& CH6_value;
        int& CH7_value;
        int& CH8_value;
 signals:

public slots:

    void keepAlive2();
private:
    QSerialPort serialPort;
    bool recording;
    ControllRecord recordData;
};

#endif // QCOMCONTROLLER_H
