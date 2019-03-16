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
#include <controlsMixer.h>
#include <flowFabricControlWidget.h>
#include <graphPlotDialog.h>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <copter/quad.h>

#include "clientSender.h"
#include "copterInputsWidget.h"
#include "frameProcessor.h"
#include "protoautopilot.h"
#include "qComController.h"


#include "controlRecord.h"
#include "joystickInput.h"
#include "simulation.h"
#include "core/geometry/mesh3DDecorated.h"
#include "mesh3DScene.h"

namespace Ui {
    class PhysicsMainWidget;
}



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
    ProtoAutoPilot iiAutoPilot;
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
    Mesh3DScene *mesh = NULL;

/** About: We have to use it to satisfy the icons license **/
    AboutDialog mAboutWidget;    

public slots:
    void settingsWidget();
    void aboutWidget();
    void keepAlive();

/** Camera **/
public:
    FrameProcessor *mProcessor = NULL;
    CapSettingsDialog mCameraParametersWidget;
    CameraModelParametersControlWidget mModelParametersWidget;
    CameraModel mCameraModel;

public slots:
    /* Add paused and stop ASAP */
    void startCamera();
    void showCameraParametersWidget();
    void showCameraModelWidget();

/** Processing **/
public:
    FlowFabricControlWidget mFlowFabricControlWidget;
    GraphPlotDialog mGraphDialog;

public slots:
    void showProcessingParametersWidget();
    void showGraphDialog();

/** Quad **/
public:
    JoystickState joystickState;
    ControlsMixer mixer;
    CopterInputs inputs;
    QTimer copterTimer;
    Quad copter;

public slots:
    /* Let it be here so far */
    void startSimuation();

    void mainAction();
    void joystickUpdated(JoystickState state);

/** UI show block **/
public:
    std::mutex uiMutex;
    std::vector<DrawRequestData *> uiQueue;

public slots:
    void updateUi();
    void keepAliveJoyStick();


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

    void on_updateCameraButton_clicked();

    void on_comboBox_2_currentTextChanged(const QString &arg1);

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
    int currentSendMode=-1;                                                //tumbler beetwen joystick and autopilot (0- js, 1-autoP)
    int frameCounter=0;                                                    //we need it in the timer
    /** Replace this with mixer **/
    JoyStickInput joystick1 ;

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
    CopterInputs joyStickOutput;                                  //for joystickValues
    CopterInputs iiOutput;                                        //for autopilot values

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
    bool cameraActive=false;
    bool rtPressed=false;
    bool ltPressed=false;

    bool recording=false;

    void sendOurValues(std::vector<uint8_t> OurValues);
    bool virtuaModeActive=false;
    std::string inputCameraPath="v4l2:/dev/video0";
    ControlRecord recordData;

    Simulation simSim;
    ClientSender virtualSender;

    int countOfSticks=0;

    bool autopilotMode=false;
    stack<Message> autopilotStack;
};

#endif // PHYSICSMAINWIDGET_H
