#ifndef PHYSICSMAINWIDGET_H
#define PHYSICSMAINWIDGET_H

#include <stack>
#include <linux/joystick.h>
#include <fcntl.h>

#include <JoystickOptionsWidget.h>
#include <QWidget>
#include <aboutDialog.h>
#include <cameraModelParametersControlWidget.h>
#include <capSettingsDialog.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "clientSender.h"
#include "copterInputsWidget.h"
#include "qComController.h"


#include "controlRecord.h"
#include "joystickInput.h"
#include "simulation.h"
#include "core/geometry/mesh3DDecorated.h"
#include "mesh3DScene.h"

namespace Ui {
    class PhysicsMainWidget;
}



class ImageCaptureInterfaceQt;
class PhysicsMainWidget;


/**
 * Ok this a draft. In general most probably we should not depend on QThread.
 **/
class FrameProcessor : public QThread
{
    Q_OBJECT
public:

    PhysicsMainWidget *target = NULL;
    ImageCaptureInterfaceQt *input = NULL;
    FrameProcessor(QObject *parent = 0) : QThread(parent)
    {}

public slots:
    /** NB: This is a place to process video **/
    void processFrame(frame_data_t frameData);


    virtual void run()
    {
        exec();
    }
};





/** This is a draft **/
class DrawRequestData
{
public:
    Mesh3DDecorated *mMesh  = NULL;
    RGB24Buffer     *mImage = NULL;

    ~DrawRequestData()
    {
        delete_safe(mMesh);
        delete_safe(mImage);
    }

};


class PhysicsMainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PhysicsMainWidget(QWidget *parent = 0);
    QSerialPort serialPort;

    QByteArray *flyCommandWriteData = NULL;
    QByteArray getDataFromSticks();

    int currentMode=0;
    bool realModeActive = false;
    bool virtualModeActive = false;

    struct AxisState {
        short x;
        short y;
    };

    void disconnectFromCopter();

    virtual ~PhysicsMainWidget();


/** Joystick **/
public:
    JoystickOptionsWidget mJoystickSettings;
    Mesh3DScene *mesh;
    /** We have to use it to satisfy the icons license **/
    AboutDialog mAboutWidget;

public slots:
    void settingsWidget();
    void aboutWidget();
    void keepAlive();

/** Camera **/
public:
    FrameProcessor *processor = NULL;
    CapSettingsDialog mCameraParametersWidget;
    CameraModelParametersControlWidget mModelParametersWidget;
    CameraModel mCameraModel;

public slots:
    /* Add paused and stop ASAP */
    void startCamera();
    void showCameraParametersWidget();
    void showCameraModelWidget();

/** UI show block **/
public:
    std::mutex uiMutex;
    std::vector<DrawRequestData *> uiQueue;

public slots:
    void updateUi();



private slots:



    void startVirtualMode();

#if 0 /*Use one slot for all channels */
    void yawChange(int i);
    void rollChange(int i);
    void pitchChange(int i);
    void throttleChange(int i);
    void CH5Change(int i);
    void CH6Change(int i);
    void CH7Change(int i);
    void CH8Change(int i);
#endif
    void startJoyStickMode();


    void onStartVirtualModeReleased();

    //void on_pushButton_released();

    void sendJoyValues();
    void startRealMode();
    // void BindToRealDrone();
    void stop();
    void onPushButton2Clicked();


    void on_comboBox_currentTextChanged(const QString &arg1);

private:
    struct Message {
        int throttle;
        int roll;
        int yaw;
        int pitch ;
        int countOfRepeats;
    };

    std::list<Message> messages;

    CopterInputs copterInputs;

    /** Replace this with mixer **/
    JoyStickInput joystick1 {
        copterInputs.axis[CopterInputs::CHANNEL_YAW],
        copterInputs.axis[CopterInputs::CHANNEL_ROLL],
        copterInputs.axis[CopterInputs::CHANNEL_PITCH],
        copterInputs.axis[CopterInputs::CHANNEL_THROTTLE],

        copterInputs.axis[CopterInputs::CHANNEL_4],
        copterInputs.axis[CopterInputs::CHANNEL_5],
        copterInputs.axis[CopterInputs::CHANNEL_6],
        copterInputs.axis[CopterInputs::CHANNEL_7]
    };

    QComController ComController {this,
        copterInputs.axis[CopterInputs::CHANNEL_YAW],
        copterInputs.axis[CopterInputs::CHANNEL_ROLL],
        copterInputs.axis[CopterInputs::CHANNEL_PITCH],
        copterInputs.axis[CopterInputs::CHANNEL_THROTTLE],

        copterInputs.axis[CopterInputs::CHANNEL_4],
        copterInputs.axis[CopterInputs::CHANNEL_5],
        copterInputs.axis[CopterInputs::CHANNEL_6],
        copterInputs.axis[CopterInputs::CHANNEL_7]
    };

    Ui::PhysicsMainWidget *ui;

    int throttleValueFromJS = 1500;
    int midThrottle = 1350;

    int counter;
    int sign(int val);
    clock_t startTime;
    void showValues( );
    void frameValuesUpdate();
    bool created=false;
    bool bind=false;
    bool arming=false;
    bool startFly=false;   //to set mid throttle after arming

    bool rtPressed=false;
    bool ltPressed=false;

    bool recording=false;

    void sendOurValues(std::vector<uint8_t> OurValues);
    bool virtuaModeActive=false;

    ControlRecord recordData;

    Simulation simSim;
    ClientSender virtualSender;

    int countOfSticks=0;

    bool autopilotMode=false;
    stack<Message> autopilotStack;
};

#endif // PHYSICSMAINWIDGET_H
