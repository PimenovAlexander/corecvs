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
#include "QWidget"
#include <fstream>
#include "ui_cloudViewDialog.h"

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
        ui->comboBox->addItem("RT/LT Usial mode");
        ui->comboBox->addItem("RT/LT Full mode");

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
            ComController.BindToRealDrone();
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
        if(arg1=="RT/LT Usial mode")
        {
            joystick1.SetRTLTUsialMode();
            throttle_value=mid_Throttle;
        }
        if(arg1=="RT/LT Full mode")
        {
            joystick1.SetRTLTFullMode();
            throttle_value=mid_Throttle;
        }
    }

