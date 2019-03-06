#ifndef QCOMCONTROLLER_H
#define QCOMCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include "controlRecord.h"
static  int v=1500;
static  int throtV=1100;


class QComController : public QObject
{
    Q_OBJECT
public:
    explicit QComController(QObject *parent = nullptr, int &_yawValue = v, int &_rollValue= v, int &_pitchValue= v, int &_throttleValue= throtV,
                            int &_CH5Value= v, int &_CH6Value= v, int &_CH7Value= v, int &_CH8Value= v);
    void bindToRealDrone();
    void sendOurValues(std::vector<uint8_t> OurValues);


    int& yawValue;
    int& rollValue;
    int& pitchValue;
    int& throttleValue;
    int& CH5Value;
    int& CH6Value;
    int& CH7Value;
    int& CH8Value;
signals:

public slots:

    void keepAlive2();
private:
    QSerialPort serialPort;
    bool recording;
    ControlRecord recordData;
};



class QComController1 {

    static const int CHANNEL_NUMBER = 8;
    int channels[CHANNEL_NUMBER ] = {};


    void pack(void);


};


#endif // QCOMCONTROLLER_H
