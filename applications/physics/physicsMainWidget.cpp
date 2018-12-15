#include "core/utils/global.h"
#include "physicsMainWidget.h"
#include "ui_physicsMainWidget.h"
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

    using namespace std;



    PhysicsMainWidget::PhysicsMainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PhysicsMainWidget)
    {
        yaw_value=1500;
        roll_value=1500;
        pitch_value=1500;
        throttle_value=1100;
        throttle_value_from_JS=1500;
        counter=0;
        ui->setupUi(this);
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

    void PhysicsMainWidget::FrameValuesUpdate()
    {
        std::thread thr([this]()
        {
            while(true)
            {
                ui->Yaw->setValue(yaw_value);
                ui->Throttle->setValue(throttle_value);

                ui->label->setText("Yaw-"+QString::number(yaw_value));
                ui->label_4->setText("throttle-"+QString::number(throttle_value));

                ui->Pitch->setValue(pitch_value);
                ui->Roll->setValue(roll_value);

                ui->label_2->setText("Roll-"+QString::number(roll_value));
                ui->label_3->setText("pitch-"+QString::number(pitch_value));

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
        counter++;
        if (!bind)                                           //119 ticks per sec
        {

        throttle_value+=sign(throttle_value_from_JS-1500);
        if (throttle_value>1800){throttle_value=1799;}
        if (throttle_value<900){throttle_value=901;}



        std::vector<uint8_t>  FlyCommandFromUs = {0x55, 0x0F, 0x19, 0x00,  0x00, 0x04, 0x20, 0x00, 0x00, 0x0F, 0x10, 0x00, 0x02, 0x10, 0x80, 0x00, 0x04, 0x20, 0x00, 0x01, 0x08, 0x40, 0x00, 0x02, 0x10, 0x80};

        int k=throttle_value-858;                    //864- min value (why?!)
        k=k*8/10;
        unsigned short sh1=k;
        bitset<32> bitsroll{sh1};

        bitset<8> firstbyte {0};
        bitset<8> secondbyte {0};
        bitset<8> thirdbyte {0};
        bitset<8> fourthbyte {0};
        bitset<8> fifthbyte {0};
        bitset<8> sixthbyte {0};



        for (int i=0;i<7;i++)
        {
            firstbyte[i+1]=bitsroll[i];
        }
        secondbyte[0]=bitsroll[7];
        secondbyte[1]=bitsroll[8];
        secondbyte[2]=bitsroll[9];                //do not ask how I realized that these bits are our roll

        k=roll_value-858;
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


        k=pitch_value-858;
        k=k*8/10;
        sh1=k;
        bitset<32> bitsthrottle{sh1};

        thirdbyte[7]=bitsthrottle[0];
        for (int i=0;i<8;i++)
        {
            fourthbyte[i]=bitsthrottle[i+1];
        }
        fifthbyte[0]=bitsthrottle[9];

        k=yaw_value-858;
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

        if (fifth_CH==1917)
        {
            FlyCommandFromUs[10]=0x60;
        }
        SendOurValues(FlyCommandFromUs);
        if (Arming)
        {
            throttle_value=930;
            throttle_value_from_JS=1500;
            fifth_CH=1917;
            Arming=false;
         }
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
/*
            0.361698125000000,Async Serial,U (0x55)
            0.361818125000000,Async Serial,'15' (0x0F)
            0.361938125000000,Async Serial,'25' (0x19)
            0.362058125000000,Async Serial,'0' (0x00)
            0.362178125000000,Async Serial,'0' (0x00)
            0.362298125000000,Async Serial,'4' (0x04)
            0.362418062500000,Async Serial,' ' (0x20)
            0.362538062500000,Async Serial,'0' (0x00)
            0.362658062500000,Async Serial,'1' (0x01)
            0.362778062500000,Async Serial,'8' (0x08)
            0.362898062500000,Async Serial,@ (0x40)
            0.363018062500000,Async Serial,'0' (0x00)
            0.363138062500000,Async Serial,'2' (0x02)
            0.363258062500000,Async Serial,'16' (0x10)
            0.363378062500000,Async Serial,'128' (0x80)
            0.363498062500000,Async Serial,'0' (0x00)
            0.363618062500000,Async Serial,'4' (0x04)
            0.363738000000000,Async Serial,' ' (0x20)
            0.363858000000000,Async Serial,'0' (0x00)
            0.363978000000000,Async Serial,'1' (0x01)
            0.364098000000000,Async Serial,'8' (0x08)
            0.364218000000000,Async Serial,@ (0x40)
            0.364338000000000,Async Serial,'0' (0x00)
            0.364458000000000,Async Serial,'2' (0x02)
            0.364578000000000,Async Serial,'16' (0x10)
            0.364698000000000,Async Serial,'128' (0x80)
  */


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

    /**
     * Reads a joystick event from the joystick device.
     *
     * Returns 0 on success. Otherwise -1 is returned.
     */
    int read_event(int fd, struct js_event *event)
    {
        ssize_t bytes;

        bytes = read(fd, event, sizeof(*event));

        if (bytes == sizeof(*event))
            return 0;

        /* Error, could not read full event. */
        return -1;
    }

    /**
     * Returns the number of axes on the controller or 0 if an error occurs.
     */
    size_t get_axis_count(int fd)
    {
        __u8 axes;

        if (ioctl(fd, JSIOCGAXES, &axes) == -1)
            return 0;

        return axes;
    }

    /**
     * Returns the number of buttons on the controller or 0 if an error occurs.
     */


    size_t get_button_count(int fd)
    {
        __u8 buttons;
        if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
            return 0;

        return buttons;
    }

    /**
     * Current state of an axis.
     */
    struct axis_state {
        short x, y;
    };


    /**
     * Keeps track of the current axis state.
     *
     * NOTE: This function assumes that axes are numbered starting from 0, and that
     * the X axis is an even number, and the Y axis is an odd number. However, this
     * is usually a safe assumption.
     *
     * Returns the axis that the event indicated.
     */
    size_t get_axis_state(struct js_event *event, struct axis_state axes[3])
    {
        size_t axis = event->number / 2;

        if (axis < 3)
        {
            if (event->number % 2 == 0)
                axes[axis].x = event->value;
            else
                axes[axis].y = event->value;
        }

        return axis;
    }



    void PhysicsMainWidget::StartJoyStickMode()
    {
    std::thread thr([this]()
    {
        const int thr_const=1;
        const int roll_const=10;
        const int pit_const=10;
        const int yaw_const=10;

        int throttle_for_hang=1360;          //it changes for diff cargo weight and battery charge , so we can set it by 'a' button

        const char *device;
        int js;
        struct js_event eventtt;
        struct axis_state axes[3] = {0};
        size_t axis;
        unsigned char seven=7;
        unsigned char six=6;
        unsigned char five=5;
        unsigned char four=4;
        unsigned char three=3;
        device = "/dev/input/js0";
        js = open(device, O_RDONLY);
        if (js == -1)
        {
            perror("Could not open joystick");
            cout<<"Could not open joystick"<<endl;
        }
        else
        {
            FrameValuesUpdate();
            cout<<"JS mode has started"<<endl;
            while (read_event(js, &eventtt) == 0)
            {
                switch (eventtt.type)
                {
                case JS_EVENT_BUTTON:
                    printf("Button %u %s\n", eventtt.number, eventtt.value ? "pressed" : "released");
                    if (eventtt.number==seven && eventtt.value)                     //arming
                    {
                        printf("##################___ARMING___######################");
                        throttle_value=930;
                        throttle_value_from_JS=1500;
                        fifth_CH=1500;
                        Arming=true;
                    }

                    if (eventtt.number==seven && !eventtt.value)                     //arming , release button
                    {
                        printf("##################___ARMING___######################");
                        throttle_value=1100;
                        throttle_value_from_JS=1500;
                     }
                    if (eventtt.number==five  && eventtt.value )                     //turn of copter(if smth goes wery wery wrong)
                    {
                        fifth_CH=1500;
                    }
                    if (eventtt.number==six  && eventtt.value )                     //  all sticks to zero (if smth goes wrong)
                    {
                        throttle_value = 1100;
                        roll_value = 1500;
                        pitch_value = 1500;
                        throttle_value_from_JS = 1500;
                        yaw_value = 1500;
                    }
                    if (eventtt.number==four && eventtt.value )                     //Throttle to mid
                    {
                        throttle_value=throttle_for_hang;
                        throttle_value_from_JS=1500;
                    }
                    if (eventtt.number==three && eventtt.value )                     //Throttle to mid
                    {
                         throttle_for_hang=throttle_value;
                    }
                    break;
                case JS_EVENT_AXIS:
                    axis = get_axis_state(&eventtt, axes);
                    if (axis < 3)
                    /* printf("Axis %u at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);*/
                    {                                                //minimum axis is not 30000, but near
                        if (axis==0)
                        {
                            yaw_value = 1500+axes[axis].x/50/yaw_const;
                            throttle_value_from_JS = 1500-axes[axis].y/50/thr_const;
                            if (yaw_value>2100){yaw_value=2099;}
                            if (yaw_value<900){yaw_value=901;}
                            if (throttle_value_from_JS>1800){throttle_value_from_JS=1799;}
                            if (throttle_value_from_JS<900){throttle_value_from_JS=901;}
                        }
                        if (axis==1)
                        {
                            pitch_value = 1500 - axes[axis].y/50/pit_const;
                            roll_value = 1500 + axes[axis].x/50/roll_const;
                            if (roll_value>2100){roll_value=2099;}
                            if (roll_value<900){roll_value=901;}
                            if (pitch_value>2100){pitch_value=2099;}
                            if (pitch_value<900){pitch_value=901;}
                        }
                    }
                    break;
                default:
                    break;
                }
            }
            }
            });
            thr.detach();
           // close(js);
    }

    void PhysicsMainWidget::Bind()
    {
        bind =true;
    }


    void PhysicsMainWidget::on_pushButton_2_clicked()
    {
        STOP();
    }

    int PhysicsMainWidget::sign(int val)
    {
        int result=0;
        if (val >0){result=1;}
        if (val <0){result=-1;}
        return result;
    }
