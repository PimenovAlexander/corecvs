#include "core/utils/global.h"

#include <fstream>

#include <unistd.h>
#include <bitset>
#include <linux/joystick.h>
#include <fcntl.h>
#include <thread>
#include <time.h>

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QWidget>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QBitArray>

#include "core/geometry/mesh3DDecorated.h"
#include "mesh3DScene.h"

#include "physicsMainWidget.h"
#include "ui_physicsMainWidget.h"
#include "joystickInput.h"
#include "clientSender.h"
#include "simulation.h"

using namespace std;

PhysicsMainWidget::PhysicsMainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PhysicsMainWidget)
{

    yawValue=1500;
    rollValue=1500;
    pitchValue=1500;
    throttleValue=1100;
    CH5Value=900;
    CH6Value=1500;
    CH7Value=1500;
    CH8Value=1500;
    throttleValueFromJS=1500;
    counter=0;
    ui->setupUi(this);
    ui->comboBox->addItem("Usual mode");
    ui->comboBox->addItem("Inertia mode");
    ui->comboBox->addItem("Casual mode");
    ui->comboBox->addItem("RT/LT Usual mode");
    ui->comboBox->addItem("RT/LT Full mode");






    frameValuesUpdate();
}

PhysicsMainWidget::~PhysicsMainWidget()
{
    delete ui;
}

void PhysicsMainWidget::settingsWidget()
{
    mJoystickSettings.show();
    mJoystickSettings.raise();
}

void PhysicsMainWidget::aboutWidget()
{
    mAboutWidget.show();
    mAboutWidget.raise();
}


void PhysicsMainWidget::onStartVirtualModeReleased()

//void PhysicsMainWidget::on_pushButton_released()

{
    SYNC_PRINT(("PhysicsMainWidget::onPushButtonReleased(): called\n"));

    /*if (!virtualModeActive & !RealModeActive)
        {
        VirtualSender.Client_connect();
        virtualModeActive = true;
        }
        */
    startVirtualMode();
}

///
/// \brief PhysicsMainWidget::sendJoyValues
///Sends to joystick emulator values of sticks with tcp
void PhysicsMainWidget::sendJoyValues()
{
    if (virtualModeActive)
    {
        if (yawValue==0)
        {
            yawValue=1;
        }
        if (rollValue==0)
        {
            rollValue=1;
        }
        if (pitchValue==0)
        {
            pitchValue=1;
        }
        if (throttleValue==0)
        {
            throttleValue=1;
        }

        string ss="";
        std::string s = std::to_string(yawValue);

        ss+=s+" ";
        s = std::to_string(rollValue);
        ss+=s+" ";
        s = std::to_string(pitchValue);
        ss+=s+" ";
        s = std::to_string(throttleValue);
        ss+=s+"*";
        cout<<ss<<endl;
        VirtualSender.clientSend(ss);
    }
    else
    {
        cout<<"Please connect to the py file"<<endl;
    }
}



void PhysicsMainWidget::showValues()                                   //shows axis values to console
{
    /* Why so many convertions? */
    std::string s = std::to_string(yawValue);
    SYNC_PRINT(("yaw __%s   ", s.c_str()));

    s = std::to_string(rollValue);
    SYNC_PRINT(("roll __%s   ", s.c_str()));

    s = std::to_string(pitchValue);
    SYNC_PRINT(("pitch __%s   ", s.c_str()));

    s = std::to_string(throttleValue);
    SYNC_PRINT(("throttle __%s   ", s.c_str()));
}


void PhysicsMainWidget::yawChange(int i)                                //i - current yaw value
{
    yawValue=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}


void PhysicsMainWidget::rollChange(int i)                                //i - current roll value
{
    rollValue=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWidget::pitchChange(int i)                                //i - current pitch value
{
    pitchValue=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWidget::throttleChange(int i)                              //i - current throttle value
{
    throttleValue=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }

}

void PhysicsMainWidget::CH5Change(int i)                              //i - current throttle value
{
    CH5Value=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWidget::CH6Change(int i)                              //i - current throttle value
{
    CH6Value=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWidget::CH7Change(int i)                              //i - current throttle value
{
    CH7Value=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWidget::CH8Change(int i)                              //i - current throttle value
{
    CH8Value=i;
    if (virtualModeActive)
    {
        sendJoyValues();
    }
}

void PhysicsMainWidget::startVirtualMode()
{
    if (!virtualModeActive)
    {
        simSim = Simulation();

        virtualModeActive=true;
        SYNC_PRINT(("PhysicsMainWidget::startVirtualMode(): Adding new object to scene\n"));
        Affine3DQ copterPos = Affine3DQ::Shift(10,10,10);

        //Mesh3DDecorated *mesh = new Mesh3DDecorated;
        mesh = new Mesh3DScene;

        mesh->switchColor();

        mesh->mulTransform(copterPos);

        mesh->setColor(RGBColor::Red());
        for (int i = 0; i < simSim.mainObjects.size(); ++i)
        {
            for (int j = 0; j < simSim.mainObjects[i].spheres.size(); ++j)
            {
                mesh->addIcoSphere(simSim.mainObjects[i].spheres[j].coords, simSim.mainObjects[i].spheres[j].radius, 3);
            }
        }


        mesh->setColor(RGBColor::Yellow());
        mesh->addIcoSphere(Vector3dd( 5, 5, -3), 2, 2);
        mesh->addIcoSphere(Vector3dd(-5, 5, -3), 2, 2);

        mesh->setColor(RGBColor::Blue());
        mesh->addIcoSphere(Vector3dd( 5, -5, -3), 2, 2);
        mesh->addIcoSphere(Vector3dd(-5, -5, -3), 2, 2);
        mesh->popTransform();

        //mesh->dumpPLY("out2.ply");

        ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(mesh));
        ui->cloud->update();

        simSim.start();
        //simSim.mainObjects[0]->addForce(Vector3dd(0, 0, -9.8));
        cout<<"done"<<endl;

        //QTimer::singleShot(8, this, SLOT(keepAlive()));           UI thread crash why????????????
    }
}

void PhysicsMainWidget::keepAlive(){
    Affine3DQ copterPos = Affine3DQ::Shift(10,10,10);

    //Mesh3DDecorated *mesh = new Mesh3DDecorated;
    mesh = new Mesh3DScene;

    mesh->switchColor();
    mesh->mulTransform(copterPos);
    mesh->setColor(RGBColor::Red());

    for (int i = 0; i < simSim.mainObjects.size(); ++i)
    {
        for (int j = 0; j < simSim.mainObjects[i].spheres.size(); ++j)
        {
            mesh->addIcoSphere(simSim.mainObjects[i].spheres[j].coords, simSim.mainObjects[i].spheres[j].radius, 3);
        }
    }

    mesh->popTransform();

    ui->cloud->update();
    ui->cloud->setNewScenePointer(QSharedPointer<Scene3D>(mesh));
}

void PhysicsMainWidget::frameValuesUpdate()
{
    std::thread thr([this]()
    {
        while(true)
        {
            if (currentMode==1)
            {
                throttleValue+=sign(throttleValueFromJS-1500);
                if (throttleValue>1800){throttleValue=1799;}
                if (throttleValue<900){throttleValue=901;}
            }


            ui->Yaw->setValue(yawValue);
            ui->Throttle->setValue(throttleValue);

            ui->label->setText("Yaw-"+QString::number(yawValue));
            ui->label_4->setText("throttle-"+QString::number(throttleValue));

            ui->Pitch->setValue(pitchValue);
            ui->Roll->setValue(rollValue);

            ui->label_2->setText("Roll-"+QString::number(rollValue));
            ui->label_3->setText("pitch-"+QString::number(pitchValue));

            ui->CH5->setValue(CH5Value);
            ui->CH6->setValue(CH6Value);
            ui->CH7->setValue(CH7Value);
            ui->CH8->setValue(CH8Value);

            ui->CH5_label->setText("CH5-"+QString::number(CH5Value));
            ui->CH6_label->setText("CH6-"+QString::number(CH6Value));
            ui->CH7_label->setText("CH7-"+QString::number(CH7Value));
            ui->CH8_label->setText("CH8-"+QString::number(CH8Value));



            usleep(30000);

        }

    });
    thr.detach();

}

void PhysicsMainWidget::startRealMode()                                    //starts controlling the copter
{
    if (!virtualModeActive & !realModeActive)
    {
        ComController.bindToRealDrone();
    }
}

void PhysicsMainWidget::stop()
{
    time_t Stop_time;
    time(&Stop_time);
    double seconds = difftime(Stop_time, startTime);
    printf("The time: %f seconds\n", seconds);
    cout<<counter<<" steeps done "<<endl;
    this->close();
}





///
/// \brief PhysicsMainWidget::SendOurValues
/// \param OurValues
/// Sends our values to module (yes, it wants its own package for every byte)
void PhysicsMainWidget::sendOurValues(std::vector<uint8_t> OurValues)
{
    std::vector<uint8_t>  flyCommandOutput = OurValues;
    for (size_t i = 0; i < flyCommandOutput.size(); i++)
    {
        std::vector<uint8_t>  flyCom = { flyCommandOutput[i] };

        int k = rollValue + 36;
        k = k * 8 / 10;

        uint8_t firstbyte = 0x00;
        for (int i = 0; i < 7; i++)
        {
            int b = (k << i);
            firstbyte&1<< (i+1);
        }

        serialPort.write((const char *)flyCom.data(), flyCom.size());
        serialPort.flush();
    }
}

void PhysicsMainWidget::startJoyStickMode()
{
    joystick1.start();
    //js.Start();

}

void PhysicsMainWidget::onPushButton2Clicked()
{
    //       cout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.count_of_repeats<<endl;

    setlocale(LC_ALL, "rus");

    char buff[50];
    ifstream fin("cppstudio.txt");
    while (!fin.eof())
    {
        Message m;
        fin >> buff;
        m.pitch=std::stoi( buff);
        fin >> buff;
        m.roll=std::stoi( buff);
        fin >> buff;
        m.throttle=std::stoi( buff);
        fin >> buff;
        m.yaw=std::stoi( buff);
        fin >> buff;
        m.countOfRepeats=std::stoi( buff);
        cout<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.countOfRepeats<<endl;
        messages.push_front(m);
    }
    for (Message mm :messages)
    {
        autopilotStack.push(mm);
    }
    Message m;
    m=autopilotStack.top();
    cout<<"-_-_-"<<m.pitch<<" "<<m.roll<<" "<<m.throttle<<" "<<m.yaw<<" "<<m.countOfRepeats<<"-_-_-"<<endl;
    autopilotStack.pop();

    autopilotMode=true;

}

int PhysicsMainWidget::sign(int val)
{
    int result=0;
    if (val > 100) {result=1;}
    if (val < -100){result=-1;}
    return result;
}

void PhysicsMainWidget::on_comboBox_currentTextChanged(const QString &arg1)                 //DO NOT TOUCH IT PLEASE
{

    if(arg1=="Usual mode")    //why qstring can not be in case?!
    {
        joystick1.setUsualCurrentMode();   //I dont want errors between qstring and string
        throttleValue=1500;
    }
    if(arg1=="Inertia mode")
    {
        joystick1.setInertiaCurrentMode();
        throttleValue=midThrottle;
    }
    if(arg1=="Casual mode")
    {
        joystick1.setCasualCurrentMode();
        throttleValue=midThrottle;
    }
    if(arg1=="RT/LT Usual mode")
    {
        joystick1.setRTLTUsialMode();
        throttleValue=midThrottle;
    }
    if(arg1=="RT/LT Full mode")
    {
        joystick1.setRTLTFullMode();
        throttleValue=midThrottle;
    }
}

