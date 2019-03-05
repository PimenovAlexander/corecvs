#include "qComController.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <bitset>
#include <iostream>
#include <thread>
#include "core/utils/global.h"
#include <QTimer>
#include <unistd.h>


QComController::QComController(QObject *parent,int &_yawValue, int &_rollValue, int &_pitchValue, int &_throttleValue,
                               int &_CH5Value, int &_CH6Value, int &_CH7Value,  int &_CH8Value)
    :  QObject(parent), yawValue(_yawValue) , rollValue(_rollValue), pitchValue(_pitchValue), throttleValue(_throttleValue),
      CH5Value(_CH5Value), CH6Value(_CH6Value), CH7Value(_CH7Value), CH8Value(_CH8Value)
{  
}

void QComController::bindToRealDrone()
{
    QSerialPortInfo serialinfo;
    QString serialPortName = "/dev/ttyUSB0";
    serialPort.setPortName(serialPortName);
    serialPort.setParity(QSerialPort::EvenParity);
    const int serialPortBaudRate = QSerialPort::BaudRate(100000);

    serialPort.setBaudRate(serialPortBaudRate);
    serialPort.setStopBits(QSerialPort::StopBits(2));

    if (!serialPort.open(QIODevice::ReadWrite))
    {
        cout <<"Failed to open port, please check that /dev/ttyUSB0 exists" << endl;
    }
    else
    {
        cout<<"Single shot starteed"<<endl;
        QTimer::singleShot(8, this, SLOT(keepAlive2()));

    }
}

///
/// \brief ComController::keepAlive
/// sends values to /dev/ttyUSB0 (i hope it is our module)
void QComController::keepAlive2()
{
    //const int difference = 858;
    const int difference = 858;
    //counter++;

    /*  if (recording)
        {
            recordData.add_message(throttleValue,rollValue,yawValue,pitchValue);
        }
        if (autopilotMode)
        {
            message m=autopilotStack.top();
            autopilotStack.pop();
            if (!autopilotStack.empty())

            {
            throttleValue=m.throttle;
            rollValue=m.roll;
            yawValue=m.yaw;
            pitchValue=m.pitch;

            if (m.count_of_repeats>0)
            {
                m.count_of_repeats--;
                autopilotStack.push(m);
            }
        }
        else
        {
            autopilotMode=false;
        }
        */

    std::vector<uint8_t>  flyCommandFromUs = {0x55, 0x0F, 0x19, 0x00, 0x00,
                                              0x04, 0x20, 0x00, 0x00, 0x0F,
                                              0x10, 0x00, 0x02, 0x10, 0x80,
                                              0x00, 0x04, 0x20, 0x00, 0x01,
                                              0x08, 0x40, 0x00, 0x02, 0x10, 0x80};

    int k = throttleValue - difference;                    //858- min value
    k = k * 8 / 10;
    unsigned short sh1 = k;
    bitset<32> bitsRoll {sh1};

    bitset<8> firstByte {0};
    bitset<8> secondByte {0};
    bitset<8> thirdByte {0};
    bitset<8> fourthByte {0};
    bitset<8> fifthByte {0};
    bitset<8> sixthByte {0};
    bitset<8> seventhByte {0};
    bitset<8> eigthByte {0};
    bitset<8> ninethByte{0};
    bitset<8> tenthByte{0};
    bitset<8> eleventhByte{0};
    bitset<8> twelfthByte{0};


    for (int i=0;i<7;i++)
    {
        firstByte[i+1]=bitsRoll[i];
    }
    secondByte[0]=bitsRoll[7];
    secondByte[1]=bitsRoll[8];
    secondByte[2]=bitsRoll[9];

    k=rollValue-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitspitch{sh1};
    for (int i=0;i<4;i++)
    {
        secondByte[i+4]=bitspitch[i];
    }
    for (int i=0;i<7;i++)
    {
        thirdByte[i]=bitspitch[i+4];
    }


    k=pitchValue-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsthrottle{sh1};

    thirdByte[7]=bitsthrottle[0];
    for (int i=0;i<8;i++)
    {
        fourthByte[i]=bitsthrottle[i+1];
    }
    fifthByte[0]=bitsthrottle[9];

    k=yawValue-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsyaw{sh1};

    for (int i=0;i<6;i++)
    {
        fifthByte[i+2]=bitsyaw[i];
    }
    for (int i=0;i<5;i++)
    {
        sixthByte[i]=bitsyaw[i+6];
    }

    k=CH5Value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsCH5{sh1};

    for (int i=0;i<7;i++)
    {
        seventhByte[i]=bitsCH5[i+3];
    }
    for (int i=0;i<3;i++)
    {
        sixthByte[i+5]=bitsCH5[i];
    }

    k=CH6Value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsCH6{sh1};

    for (int i=0;i<8;i++)
    {
        eigthByte[i]=bitsCH6[i];
    }
    for (int i=0;i<3;i++)
    {
        ninethByte[i]=bitsCH6[i+8];
    }

    k=CH7Value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsCH7{sh1};

    for (int i=0;i<6;i++)
    {
        ninethByte[i+3]=bitsCH7[i];
    }
    for (int i=0;i<7;i++)
    {
        tenthByte[i]=bitsCH7[i+5];
    }

    k=CH8Value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsCH8{sh1};

    for (int i=0;i<8;i++)
    {
        eleventhByte[i]=bitsCH8[i+2];
    }
    tenthByte[7]=bitsCH8[1];
    tenthByte[6]=bitsCH8[0];
    twelfthByte[0]=bitsCH8[10];

    uint8_t fb=firstByte.to_ulong();
    flyCommandFromUs[4]=fb;

    fb=secondByte.to_ulong();
    flyCommandFromUs[5]=fb;

    fb=thirdByte.to_ulong();
    flyCommandFromUs[6]=fb;

    fb=fourthByte.to_ulong();
    flyCommandFromUs[7]=fb;

    fb=fifthByte.to_ulong();
    flyCommandFromUs[8]=fb;

    fb=sixthByte.to_ulong();
    flyCommandFromUs[9]=fb;

    fb=seventhByte.to_ulong();
    flyCommandFromUs[10]=fb;

    fb=eigthByte.to_ulong();
    flyCommandFromUs[11]=fb;

    fb=ninethByte.to_ulong();
    flyCommandFromUs[12]=fb;

    fb=tenthByte.to_ulong();
    flyCommandFromUs[13]=fb;


    fb=eleventhByte.to_ulong();
    flyCommandFromUs[14]=fb;


    fb=twelfthByte.to_ulong();
    flyCommandFromUs[15]=fb;

    sendOurValues(flyCommandFromUs);


    QTimer::singleShot(8, this, SLOT(keepAlive2()));
}

///
/// \brief ComController::SendOurValues
/// \param OurValues
/// Sends our values to module (yes, it wants its own package for every Byte)
void QComController::sendOurValues(std::vector<uint8_t> OurValues)
{
    std::vector<uint8_t>  FlyCommandOutput = OurValues;
    for (int i=0;i<FlyCommandOutput.size();i++)
    {
        std::vector<uint8_t>  FlyCom={FlyCommandOutput[i]};

        int k=rollValue+36;
        k=k*8/10;
        uint8_t firstByte=0x00;
        for (int i=0;i<7;i++)
        {
            int b=k<<i;
            firstByte&1<<i+1;
        }

        QByteArray *qBytes =  new QByteArray(reinterpret_cast<const char*>(FlyCom.data()),FlyCom.size());

        serialPort.write(*qBytes);
        serialPort.flush();
    }
}
