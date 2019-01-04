#include "qcomcontroller.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <bitset>
#include <iostream>
#include <thread>
#include "core/utils/global.h"
#include <QTimer>
#include <unistd.h>


QComController::QComController(QObject *parent,int &_yaw_value, int &_roll_value, int &_pitch_value, int &_throttle_value, int &_CH5_value, int &_CH6_value, int &_CH7_value,  int &_CH8_value )
    :  QObject(parent), yaw_value(_yaw_value) , roll_value(_roll_value),pitch_value(_pitch_value), throttle_value(_throttle_value),CH5_value(_CH5_value),CH6_value(_CH6_value),CH7_value(_CH7_value),CH8_value(_CH8_value)


{

}

void QComController::BindToRealDrone()
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
            recordData.add_message(throttle_value,roll_value,yaw_value,pitch_value);
        }
        if (autopilotMode)
        {
            message m=autopilotStack.top();
            autopilotStack.pop();
            if (!autopilotStack.empty())

            {
            throttle_value=m.throttle;
            roll_value=m.roll;
            yaw_value=m.yaw;
            pitch_value=m.pitch;

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

    std::vector<uint8_t>  FlyCommandFromUs = {0x55, 0x0F, 0x19, 0x00,  0x00, 0x04, 0x20, 0x00, 0x00, 0x0F, 0x10, 0x00, 0x02, 0x10, 0x80, 0x00, 0x04, 0x20, 0x00, 0x01, 0x08, 0x40, 0x00, 0x02, 0x10, 0x80};

    int k=throttle_value-difference;                    //858- min value
    k=k*8/10;
    unsigned short sh1=k;
    bitset<32> bitsroll{sh1};

    bitset<8> firstbyte {0};
    bitset<8> secondbyte {0};
    bitset<8> thirdbyte {0};
    bitset<8> fourthbyte {0};
    bitset<8> fifthbyte {0};
    bitset<8> sixthbyte {0};
    bitset<8> seventhbyte {0};
    bitset<8> eigthbyte {0};
    bitset<8> ninethbyte{0};
    bitset<8> tenthbyte{0};
    bitset<8> eleventhbyte{0};
    bitset<8> twelfthbyte{0};


    for (int i=0;i<7;i++)
    {
        firstbyte[i+1]=bitsroll[i];
    }
    secondbyte[0]=bitsroll[7];
    secondbyte[1]=bitsroll[8];
    secondbyte[2]=bitsroll[9];

    k=roll_value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitspitch{sh1};
    for (int i=0;i<4;i++)
    {
        secondbyte[i+4]=bitspitch[i];
    }
    for (int i=0;i<7;i++)
    {
        thirdbyte[i]=bitspitch[i+4];
    }


    k=pitch_value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsthrottle{sh1};

    thirdbyte[7]=bitsthrottle[0];
    for (int i=0;i<8;i++)
    {
        fourthbyte[i]=bitsthrottle[i+1];
    }
    fifthbyte[0]=bitsthrottle[9];

    k=yaw_value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsyaw{sh1};

    for (int i=0;i<6;i++)
    {
       fifthbyte[i+2]=bitsyaw[i];
    }
    for (int i=0;i<5;i++)
    {
       sixthbyte[i]=bitsyaw[i+6];
    }

    k=CH5_value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsCH5{sh1};

    for (int i=0;i<7;i++)
    {
       seventhbyte[i]=bitsCH5[i+3];
    }
    for (int i=0;i<3;i++)
    {
       sixthbyte[i+5]=bitsCH5[i];
    }

    k=CH6_value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsCH6{sh1};

    for (int i=0;i<8;i++)
    {
       eigthbyte[i]=bitsCH6[i];
    }
    for (int i=0;i<3;i++)
    {
       ninethbyte[i]=bitsCH6[i+8];
    }

    k=CH7_value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsCH7{sh1};

    for (int i=0;i<6;i++)
    {
       ninethbyte[i+3]=bitsCH7[i];
    }
    for (int i=0;i<7;i++)
    {
       tenthbyte[i]=bitsCH7[i+5];
    }

    k=CH8_value-difference;
    k=k*8/10;
    sh1=k;
    bitset<32> bitsCH8{sh1};

    for (int i=0;i<8;i++)
    {
       eleventhbyte[i]=bitsCH8[i+2];
    }
    tenthbyte[7]=bitsCH8[1];
    tenthbyte[6]=bitsCH8[0];
    twelfthbyte[0]=bitsCH8[10];

    uint8_t fb=firstbyte.to_ulong();
    FlyCommandFromUs[4]=fb;

    fb=secondbyte.to_ulong();
    FlyCommandFromUs[5]=fb;

    fb=thirdbyte.to_ulong();
    FlyCommandFromUs[6]=fb;

    fb=fourthbyte.to_ulong();
    FlyCommandFromUs[7]=fb;

    fb=fifthbyte.to_ulong();
    FlyCommandFromUs[8]=fb;

    fb=sixthbyte.to_ulong();
    FlyCommandFromUs[9]=fb;

    fb=seventhbyte.to_ulong();
    FlyCommandFromUs[10]=fb;

    fb=eigthbyte.to_ulong();
    FlyCommandFromUs[11]=fb;

    fb=ninethbyte.to_ulong();
    FlyCommandFromUs[12]=fb;

    fb=tenthbyte.to_ulong();
    FlyCommandFromUs[13]=fb;


    fb=eleventhbyte.to_ulong();
     FlyCommandFromUs[14]=fb;


    fb=twelfthbyte.to_ulong();
     FlyCommandFromUs[15]=fb;

    SendOurValues(FlyCommandFromUs);


    QTimer::singleShot(8, this, SLOT(keepAlive2()));
}

///
/// \brief ComController::SendOurValues
/// \param OurValues
/// Sends our values to module (yes, it wants its own package for every byte)
void QComController::SendOurValues(std::vector<uint8_t> OurValues)
{
    std::vector<uint8_t>  FlyCommandOutput = OurValues;
    for (int i=0;i<FlyCommandOutput.size();i++)
    {
        std::vector<uint8_t>  FlyCom={FlyCommandOutput[i]};

        int k=roll_value+36;
        k=k*8/10;
        uint8_t firstbyte=0x00;
        for (int i=0;i<7;i++)
        {
           int b=k<<i;
           firstbyte&1<<i+1;
        }

    QByteArray *qbytes =  new QByteArray(reinterpret_cast<const char*>(FlyCom.data()),FlyCom.size());

    serialPort.write(*qbytes);
    serialPort.flush();
    }
}
