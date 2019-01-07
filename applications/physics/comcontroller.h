#ifndef COMCONTROLLER_H
#define COMCONTROLLER_H

#include "controllrecord.h"

#include <QSerialPort>



class ComController
{
public:
    ComController(int &_yaw_value, int &_roll_value, int &_pitch_value, int &_throttle_value, int &_CH5_value, int &_CH6_value, int &_CH7_value,  int &_CH8_value );
    void BindToRealDrone();
    void SendOurValues(std::vector<uint8_t> OurValues);

    int& yaw_value;
    int& roll_value;
    int& pitch_value;
    int& throttle_value;
    int& CH5_value;
    int& CH6_value;
    int& CH7_value;
    int& CH8_value;
public slots:

    void keepAlive2();
private:
    QSerialPort serialPort;
    bool recording;
    ControllRecord recordData;




};

#endif // COMCONTROLLER_H
