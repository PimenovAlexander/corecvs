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
        throttle_value_from_JS=1500;
        counter=0;
        ui->setupUi(this);
        /*ui->comboBox->addItem("Usual mode");
        ui->comboBox->addItem("Inertia mode");
        ui->comboBox->addItem("Casual mode");*/
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

        int k=throttle_value-858;                    //858- min value
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
        secondbyte[2]=bitsroll[9];

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
    struct axis_state {        short x, y;   };


    /**
     * Keeps track of the current axis state.
     *
     * NOTE: This function assumes that axes are numbered starting from 0, and that
     * the X axis is an even number, and the Y axis is an odd number. However, this
     * is usually a safe assumption.
     *
     * Returns the axis that the event indicated.
     */

    size_t get_axis_state(struct js_event *event,  PhysicsMainWidget::axis_state axes[3])
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
                autopilotMode=false;
                switch (eventtt.type)
                {
                case JS_EVENT_BUTTON:
                    switch (Current_mode)
                    {
                        case 0:
                            usial_buttons(eventtt);
                            break;
                        case 1:
                            inertial_buttons(eventtt);
                            break;
                        case 2:
                            casual_buttons(eventtt);
                            break;
                        default: break;
                    }

                  /*   if (eventtt.number==seven && eventtt.value)                     //arming
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
                    }*/
                    break;
                case JS_EVENT_AXIS:
                    switch (Current_mode)
                    {
                        case 0:
                            usial_sticks(eventtt);
                            break;
                        case 1:
                            inertial_sticks(eventtt);
                            break;
                        case 2:
                            casual_sticks(eventtt);
                            break;
                        default: break;
                    }
                    /*axis = get_axis_state(&eventtt, axes);
                    if (axis < 3)
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
                    }*/
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

    void PhysicsMainWidget::usial_buttons(js_event event)
    {
        unsigned char seven=7;
        unsigned char six=6;
        unsigned char five=5;
        unsigned char four=4;
        unsigned char three=3;
        unsigned char two=2;
        unsigned char one=1;
        if (event.number==seven)                     //arming
        {
           Start_arming(event.value);
        }
        if (event.number==five  && event.value )                     //turn of copter(if smth goes wery wery wrong)
        {
            disconnect_from_copter();
        }
        if (event.number==one && event.value)                     //arming
        {
            if (!recording)
            {
                StartRecord();
            }
            else
            {
                StopRecord();
            }
        }
    }

    void PhysicsMainWidget::usial_sticks(js_event event)
    {
        const int thr_const=1;
        const int roll_const=10;
        const int pit_const=10;
        const int yaw_const=10;

         size_t axis;
         axis = get_axis_state(&event, axes);
        if (axis < 3)
        //printf("Axis %u at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);
        {                                                //minimum axis is not 30000, but near
            if (axis==0)
            {
                yaw_value = 1500+axes[axis].x/50/yaw_const;
                throttle_value = 1500-axes[axis].y/50/thr_const;
                if (yaw_value>2100){yaw_value=2099;}
                if (yaw_value<900){yaw_value=901;}
                if (throttle_value>2100){throttle_value=2099;}
                if (throttle_value<900) {throttle_value=901;}

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
    }

    void PhysicsMainWidget::inertial_buttons(js_event event)
    {
        unsigned char seven=7;
        unsigned char six=6;
        unsigned char five=5;
        unsigned char four=4;
        unsigned char three=3;
        unsigned char two=2;
        unsigned char one=1;
        if (event.number==seven )                     //arming
        {
           Start_arming(event.value);
        }
        if (event.number==five  && event.value )                     //turns of copter(if smth goes very very wrong)
        {
            disconnect_from_copter();
        }
        if (event.number==six  && event.value )                     //  all sticks to zero (if smth goes wrong)
        {
            throttle_value = 1100;
            roll_value = 1500;
            pitch_value = 1500;
            throttle_value_from_JS = 1500;
            yaw_value = 1500;
        }
        if (event.number==four && event.value )                     //Throttle to mid
        {
            throttle_value=mid_Throttle;
            throttle_value_from_JS=1500;
        }
        if (event.number==three && event.value )                     //Throttle to mid
        {
             mid_Throttle=throttle_value;
        }
        if (event.number==one && event.value)                     //arming
        {
            if (!recording)
            {
                StartRecord();
            }
            else
            {
                StopRecord();
            }
        }
    }

    void PhysicsMainWidget::inertial_sticks(js_event event)
    {
        const int thr_const=1;
        const int roll_const=10;
        const int pit_const=10;
        const int yaw_const=10;

        size_t axis;
        axis = get_axis_state(&event, axes);
        if (axis < 3)
        /* printf("Axis %u at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);*/
        {                                                //minimum axis is not 30000, but near
            if (axis==0)
            {
                yaw_value = 1500+axes[axis].x/50/yaw_const;
                throttle_value_from_JS = 1500-axes[axis].y/50/thr_const;
                if (yaw_value>2100){yaw_value=2099;}
                if (yaw_value<900){yaw_value=901;}
                if (throttle_value>2100){throttle_value=2099;}
                if (throttle_value<900) {throttle_value=901;}

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
    }

    void PhysicsMainWidget::casual_buttons(js_event event)
    {

        unsigned char seven=7;
        unsigned char six=6;
        unsigned char five=5;
        unsigned char four=4;
        unsigned char three=3;
        unsigned char two=2;
        unsigned char one=1;
        if (event.number==five )                     //arming
        {
           Start_arming(event.value);
        }
        if (event.number==four  && event.value )                     //turn of copter(if smth goes wery wery wrong)
        {
            disconnect_from_copter();
        }
        if (event.number==seven  && event.value && !lt_pressed)
        {
            rt_pressed=true;
            throttle_value=1500;
        }
        if (event.number==seven  && !event.value && !lt_pressed)
        {
            rt_pressed=false;
            throttle_value=mid_Throttle;
        }
        if (event.number==six  && event.value && !rt_pressed)
        {
            lt_pressed=true;
            throttle_value=1250;
        }
        if (event.number==six  && !event.value && !rt_pressed)
        {
            lt_pressed=false;
            throttle_value=mid_Throttle;
        }
        if (event.number==one && event.value)                     //arming
        {
            if (!recording)
            {
                StartRecord();
            }
            else
            {
                StopRecord();
            }
        }
    }

    void PhysicsMainWidget::casual_sticks(js_event event)
    {
        const int thr_const=1;
        const int roll_const=10;
        const int pit_const=10;
        const int yaw_const=10;

        size_t axis;

        axis = get_axis_state(&event, axes);
        if (axis < 3)
        /* printf("Axis %u at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);*/
        {                                                //minimum axis is not 30000, but near
            if (axis==0)
            {
                pitch_value = 1500 - axes[axis].y/50/pit_const;
                roll_value = 1500 + axes[axis].x/50/roll_const;
                if (roll_value>2100){roll_value=2099;}
                if (roll_value<900){roll_value=901;}
                if (pitch_value>2100){pitch_value=2099;}
                if (pitch_value<900){pitch_value=901;}

            }
            if (axis==1)
            {
                yaw_value = 1500 + axes[axis].x/50/roll_const;
                if (yaw_value>2100){yaw_value=2099;}
                if (yaw_value<900){yaw_value=901;}

            }
        }
    }

    void PhysicsMainWidget::Start_arming(bool pressed)
    {
        if (pressed)
        {
            printf("##################___ARMING___######################");
            throttle_value=930;
            throttle_value_from_JS=1500;
            fifth_CH=1500;
            Arming=true;
        }
        else
        {
            printf("##################___ARMING___######################");
            throttle_value=1100;
            throttle_value_from_JS=1500;
        }
    }

    void PhysicsMainWidget::disconnect_from_copter()
    {
        fifth_CH=1500;
    }

    void PhysicsMainWidget::Bind()
    {
        bind =true;
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
        if (val >0){result=1;}
        if (val <0){result=-1;}
        return result;
    }

    void PhysicsMainWidget::on_comboBox_currentTextChanged(const QString &arg1)
    {
        if(arg1=="Usual mode")    //why qstring can not be in case?!
        {
            Current_mode=0;   //I dont want errors between qstring and string
            throttle_value=1500;

        }
        if(arg1=="Inertia mode")
        {
            Current_mode=1;
            throttle_value=mid_Throttle;

        }
        if(arg1=="Casual mode")
        {
            Current_mode=2;
            throttle_value=mid_Throttle;
        }
    }



    void PhysicsMainWidget::StartRecord()
    {
        recording=true;
        recordData =  ControllRecord();

    }

    void PhysicsMainWidget::StopRecord()
    {
        printf("##################___writing___######################");

        recording=false;
        recordData.Save();
    }
