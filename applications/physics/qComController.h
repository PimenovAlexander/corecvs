#ifndef QCOMCONTROLLER_H
#define QCOMCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include "controlRecord.h"
#include "copterInputs.h"
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

    bool mutexActive=false;
    CopterInputs input;
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
    CopterInputs output;                     //in case of mutex we will send old value per 1 time
    QSerialPort serialPort;
    bool recording;
    ControlRecord recordData;
    void updateOutput();
};


class QComController1 {

    enum ChannelID {
        CHANNEL_0,
        CHANNEL_THROTTLE = CHANNEL_0,
        CHANNEL_1,
        CHANNEL_ROLL     = CHANNEL_1,
        CHANNEL_2,
        CHANNEL_PITCH    = CHANNEL_2,
        CHANNEL_3,
        CHANNEL_YAW      = CHANNEL_3,


        CHANNEL_4,
        CHANNEL_5,
        CHANNEL_6,
        CHANNEL_7,

        CHANNEL_LAST
    };

    /** Each channel is 10 bit in FrSky **/
    const int MASK = 0x3FF;

    int16_t channels[CHANNEL_LAST] = {};


    void pack(void);


};


#endif // QCOMCONTROLLER_H
