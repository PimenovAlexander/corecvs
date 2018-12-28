#include "core/utils/global.h"
#include "physicsMainWidget.h"
#include "ui_physicsMainWidget.h"
#include "joystickinput.h"
#include "clientsender.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <unistd.h>
#include <QBitArray>
#include <bitset>
#include <linux/joystick.h>
#include <fcntl.h>
#include <thread>
#include "time.h"
#include <fstream>

    using namespace std;



    PhysicsMainWidget::PhysicsMainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PhysicsMainWidget)
    {
        yaw_value=1500;
        roll_value=1500;
        pitch_value=1500;
        throttle_value=1100;
        CH5_value=900;
        CH6_value=1500;
        CH7_value=1500;
        CH8_value=1500;
        throttle_value_from_JS=1500;
        counter=0;
        ui->setupUi(this);
        ui->comboBox->addItem("Usual mode");
        ui->comboBox->addItem("Inertia mode");
        ui->comboBox->addItem("Casual mode");
        FrameValuesUpdate();
    }

    PhysicsMainWidget::~PhysicsMainWidget()
    {
        delete ui;
    }

    void PhysicsMainWidget::on_pushButton_released()
    {
        if (!VirtualModeActive & !RealModeActive)
        {
        VirtualSender.Client_connect();
        VirtualModeActive = true;
        }
    }

    ///
    /// \brief PhysicsMainWidget::SendJoyValues
    ///Sends to joystick emulator values of sticks with tcp
    void PhysicsMainWidget::SendJoyValues()
    {
        if (VirtualModeActive)
        {
            if (yaw_value==0)
            {
                yaw_value=1;
            }
            if (roll_value==0)
            {
                roll_value=1;
            }
            if (pitch_value==0)
            {
                pitch_value=1;
            }
            if (throttle_value==0)
            {
                throttle_value=1;
            }

            string ss="";
            std::string s = std::to_string(yaw_value);

            ss+=s+" ";
            s = std::to_string(roll_value);
            ss+=s+" ";
            s = std::to_string(pitch_value);
            ss+=s+" ";
            s = std::to_string(throttle_value);
            ss+=s+"*";
            cout<<ss<<endl;
            VirtualSender.Client_send(ss);
            }
            else
            {
                cout<<"Please connect to the py file"<<endl;
            }
    }



    void PhysicsMainWidget::ShowValues()                                   //shows axis values to console
    {
        std::string s = std::to_string(yaw_value);
        SYNC_PRINT(("yaw __"));
        SYNC_PRINT((s.c_str()));
        SYNC_PRINT(("  "));


          s = std::to_string(roll_value);
        SYNC_PRINT(("roll __"));
        SYNC_PRINT((s.c_str()));
        SYNC_PRINT(("  "));

          s = std::to_string(pitch_value);
        SYNC_PRINT(("pitch __"));
        SYNC_PRINT((s.c_str()));
        SYNC_PRINT(("  "));

          s = std::to_string(throttle_value);
        SYNC_PRINT(("throttle __"));
        SYNC_PRINT((s.c_str()));
        SYNC_PRINT(("  "));
    }


    void PhysicsMainWidget::yawChange(int i)                                //i - current yaw value
    {
        yaw_value=i;
        if (VirtualModeActive)
        {
        SendJoyValues();
        }
     }


    void PhysicsMainWidget::rollChange(int i)                                //i - current roll value
    {
        roll_value=i;
        if (VirtualModeActive)
        {
        SendJoyValues();
        }
     }

    void PhysicsMainWidget::pitchChange(int i)                                //i - current pitch value
    {
        pitch_value=i;
        if (VirtualModeActive)
        {
        SendJoyValues();
        }
     }

    void PhysicsMainWidget::throttleChange(int i)                              //i - current throttle value
    {
        throttle_value=i;
        if (VirtualModeActive)
        {
        SendJoyValues();
        }

    }

    void PhysicsMainWidget::CH5Change(int i)                              //i - current throttle value
    {
        CH5_value=i;
        if (VirtualModeActive)
        {
        SendJoyValues();
        }
    }

    void PhysicsMainWidget::CH6Change(int i)                              //i - current throttle value
    {
        CH6_value=i;
        if (VirtualModeActive)
        {
        SendJoyValues();
        }
    }

    void PhysicsMainWidget::CH7Change(int i)                              //i - current throttle value
    {
        CH7_value=i;
        if (VirtualModeActive)
        {
        SendJoyValues();
        }
    }

    void PhysicsMainWidget::CH8Change(int i)                              //i - current throttle value
    {
        CH8_value=i;
        if (VirtualModeActive)
        {
        SendJoyValues();
        }
    }

    void PhysicsMainWidget::FrameValuesUpdate()
    {
        std::thread thr([this]()
        {
            while(true)
            {
                if (Current_mode==1)
                {
                    throttle_value+=sign(throttle_value_from_JS-1500);
                    if (throttle_value>1800){throttle_value=1799;}
                    if (throttle_value<900){throttle_value=901;}
                }


                ui->Yaw->setValue(yaw_value);
                ui->Throttle->setValue(throttle_value);

                ui->label->setText("Yaw-"+QString::number(yaw_value));
                ui->label_4->setText("throttle-"+QString::number(throttle_value));

                ui->Pitch->setValue(pitch_value);
                ui->Roll->setValue(roll_value);

                ui->label_2->setText("Roll-"+QString::number(roll_value));
                ui->label_3->setText("pitch-"+QString::number(pitch_value));

                ui->CH5->setValue(CH5_value);
                ui->CH6->setValue(CH6_value);
                ui->CH7->setValue(CH7_value);
                ui->CH8->setValue(CH8_value);

                ui->CH5_label->setText("CH5-"+QString::number(CH5_value));
                ui->CH6_label->setText("CH6-"+QString::number(CH6_value));
                ui->CH7_label->setText("CH7-"+QString::number(CH7_value));
                ui->CH8_label->setText("CH8-"+QString::number(CH8_value));



                usleep(30000);

            }

        });
        thr.detach();

    }

    void PhysicsMainWidget::StartRealMode()                                    //starts controlling the copter
    {
        if (!VirtualModeActive & !RealModeActive)
        {
            BindToRealDrone();
        }
    }

    void PhysicsMainWidget::STOP()
    {
        time_t Stop_time;
        time(&Stop_time);
        double seconds = difftime(Stop_time, start_time);
        printf("The time: %f seconds\n", seconds);
         cout<<counter<<" steeps done "<<endl;
        this->close();
    }


    void PhysicsMainWidget::BindToRealDrone()
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
            RealModeActive = true;
            time(&start_time);
            QTimer::singleShot(8, this, SLOT(keepAlive()));

        }
    }


    ///
    /// \brief PhysicsMainWidget::keepAlive
    /// sends values to /dev/ttyUSB0 (i hope it is our module)
    void PhysicsMainWidget::keepAlive()
    {
        //const int difference = 858;
        const int difference = 862;
        counter++;
        if (!bind)                                           //119 ticks per sec
        {
            if (recording)
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
            }

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

        cout<<bitsCH8<<endl;

        fb=eleventhbyte.to_ulong();
        cout<<eleventhbyte<<endl;
        FlyCommandFromUs[14]=fb;


        fb=twelfthbyte.to_ulong();
        cout<<twelfthbyte<<endl;
        FlyCommandFromUs[15]=fb;

        SendOurValues(FlyCommandFromUs);

        if (startFly)
        {
            throttle_value=1100;
            throttle_value_from_JS=1500;
            startFly=false;
        }
        }
        else
        {
            bind=false;
            std::vector<uint8_t>  FlyCommandFromUs = {0x55, 0x0F, 0x19, 0x00,  0x00, 0x04, 0x20, 0x00, 0x01, 0x08, 0x40, 0x00, 0x02, 0x10, 0x80, 0x00, 0x04, 0x20, 0x00, 0x01, 0x08, 0x40, 0x00, 0x02, 0x10, 0x80};

        }
        QTimer::singleShot(8, this, SLOT(keepAlive()));
    }



    ///
    /// \brief PhysicsMainWidget::SendOurValues
    /// \param OurValues
    /// Sends our values to module (yes, it wants its own package for every byte)
    void PhysicsMainWidget::SendOurValues(std::vector<uint8_t> OurValues)
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


    //JOYSTICKs PART

    struct axis_state {        short x, y;   };




    void PhysicsMainWidget::StartJoyStickMode()
    {
        joystick1.Start();
          //js.Start();

    }

    void PhysicsMainWidget::on_pushButton_2_clicked()
    {
        //       cout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.count_of_repeats<<endl;
        setlocale(LC_ALL, "rus");

        char buff[50];
        ifstream fin("cppstudio.txt");
        while (!fin.eof())
        {
            message m;
            fin >> buff;
            m.pitch=std::stoi( buff);
            fin >> buff;
            m.roll=std::stoi( buff);
            fin >> buff;
            m.throttle=std::stoi( buff);
            fin >> buff;
            m.yaw=std::stoi( buff);
            fin >> buff;
            m.count_of_repeats=std::stoi( buff);
            cout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.count_of_repeats<<endl;
            messages.push_front(m);
        }
        for (message mm :messages)
        {
            autopilotStack.push(mm);
        }
        message m;
        m=autopilotStack.top();
        cout<<"-_-_-"<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.count_of_repeats<<"-_-_-"<<endl;
        autopilotStack.pop();

        autopilotMode=true;
    }

    int PhysicsMainWidget::sign(int val)
    {
        int result=0;
        if (val >100){result=1;}
        if (val <-100){result=-1;}
        return result;
    }

    void PhysicsMainWidget::on_comboBox_currentTextChanged(const QString &arg1)
    {
        if(arg1=="Usual mode")    //why qstring can not be in case?!
        {
            joystick1.SetUsualCurrentMode();   //I dont want errors between qstring and string
            throttle_value=1500;

        }
        if(arg1=="Inertia mode")
        {
            joystick1.SetInertiaCurrentMode();
            throttle_value=mid_Throttle;

        }
        if(arg1=="Casual mode")
        {
            joystick1.SetCasualCurrentMode();
            throttle_value=mid_Throttle;
        }
    }

