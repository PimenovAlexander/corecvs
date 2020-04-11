#ifndef PHYSICSMAINWINDOW_H
#define PHYSICSMAINWINDOW_H

#include <QMainWindow>

#include <stack>
#include <linux/joystick.h>
#include <fcntl.h>
#include <world/simulationWorld.h>

#include <JoystickOptionsWidget.h>
#include <QWidget>
#include <statisticsDialog.h>

#include "calibration.h"
#include "cameraModelParametersControlWidget.h"
#include "capSettingsDialog.h"
#include "controlsMixer.h"
#include "flowFabricControlWidget.h"
#include "graphPlotDialog.h"
#include "inputSelectorWidget.h"
#include "radioControlWidget.h"
#include "joystickReader.h"

#include "copter/quad.h"
#include "copter/droneObject.h"

#include "calibrationWidget.h"
#include "clientSender.h"
#include "copterInputsWidget.h"
#include "frameProcessor.h"
#include "physicsAboutWidget.h"
#include "protoautopilot.h"
#include "multimoduleController.h"


#include "controlRecord.h"
#include "simulation.h"
#include "core/geometry/mesh/mesh3DDecorated.h"
#include "mesh3DScene.h"
#include "patternDetectorParametersWidget.h"


namespace Ui {
class PhysicsMainWindow;
}

/** This is a draft **/
class DrawRequestData
{
public:
    Mesh3DDecorated *mMesh  = NULL;
    RGB24Buffer     *mImage = NULL;

    Statistics      stats;

    ~DrawRequestData()
    {
        delete_safe(mMesh);
        delete_safe(mImage);
    }

};


class SceneShaded;

class PhysicsMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PhysicsMainWindow(QWidget *parent = 0);
    ~PhysicsMainWindow();

private:
    Ui::PhysicsMainWindow *ui;

    ProtoAutoPilot iiAutoPilot;
    QByteArray *flyCommandWriteData = NULL;
    QByteArray getDataFromSticks();

    int currentMode=0;
    bool realModeActive = false;
    bool virtualModeActive = false;
    void disconnectFromCopter();
    Calibration calib;
/** Joystick **/
public:
    JoystickOptionsWidget mJoystickSettings;

/** About: We have to use it to satisfy the icons license **/
    PhysicsAboutWidget mAboutWidget;

public slots:
    void showJoystickSettingsWidget();
    void showAboutWidget();
    void keepAlive();

/** Camera **/
public:
    FrameProcessor *mProcessor = NULL;
    CapSettingsDialog mCameraParametersWidget;
    CameraModelParametersControlWidget mModelParametersWidget;
    CameraModel mCameraModel;
    InputSelectorWidget mInputSelector;
    CalibrationWidget calibrationWidget;


public slots:
    void showCameraInput();

    /* Add paused and stop ASAP */
    void startCamera();
    void pauseCamera();
    void stopCamera();

    void showCameraParametersWidget();
    void showCameraModelWidget();
    void loadCameraModel(QString filename);
    void saveCameraModel(QString filename);
    void cameraModelWidgetChanged();

signals:
    void newCameraModel(CameraModel model);

/** Processing **/
public:
    FlowFabricControlWidget mFlowFabricControlWidget;
    GraphPlotDialog mGraphDialog;
    StatisticsDialog mStatsDialog;
    PatternDetectorParametersWidget patternDetectorParametersWidget;

public slots:
    void showProcessingParametersWidget();
    void showGraphDialog();
    void showStatistics();

signals:
    void newPatternDetectionParameters(GeneralPatternDetectorParameters params);

/** World **/
public:
    SimulationWorld world;

public slots:
    void worldRedraw();

/** Quad **/
public:
    JoystickState joystickState;
    ControlsMixer mixer;
    CopterInputs inputs;
    QTimer copterTimer;
    Quad copter;
    DroneObject drone;

    FlightControllerParameters currentFlightControllerParameters; /* Not sure we need it here */
    ReflectionWidget *flightControllerParametersWidget = NULL;

public slots:
    /* Let it be here so far */
    void startSimuation();

    void mainAction();
    void joystickUpdated(JoystickState state);

    void showFlightControllerParameters();
    void flightControllerParametersChanged();

    void showPatternDetectionParameters();
    void patternDetectionParametersChanged();


/** Radio */
public:
    RadioControlWidget radioWidget;
public slots:
    void showRadioControlWidget();

/** UI show block **/
public:
    std::mutex uiMutex;
    std::vector<DrawRequestData *> uiQueue;
    SceneShaded *mShadedScene = NULL;

public slots:
    void updateUi();
    void keepAliveJoyStick();

/** Model download **/
public slots:
    void downloadModels();


/** Save/load block */
public:
    vector<SaveableWidget *> toSave;


private slots:

    void stopVirtualMode();
    void startVirtualMode();

    void startJoyStickMode();


    void onStartVirtualModeReleased();

    //void on_pushButton_released();

    void sendJoyValues();
    void startRealMode();
    // void BindToRealDrone();
    void stop();
    void onPushButton2Clicked();


    /* This is just terrible */
    void on_comboBox_2_currentTextChanged(const QString &arg1);
    void on_connetToVirtualButton_released();
    void on_toolButton_3_released();
    void on_toolButton_2_released();
    void on_pushButton_released();

    void calibrateCamera();


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

    MultimoduleController multimoduleController;
    CopterInputs joyStickOutput;                                  //for joystickValues
    CopterInputs iiOutput;                                        //for autopilot values
    CopterInputs mainOutput;
    CopterInputs failSafeOutput;

    int throttleValueFromJS = 1500;
    int midThrottle = 1350;

    int counter;
    int sign(int val);
    clock_t startTime;
    void showValues( );
    void frameValuesUpdate();

    bool cameraActive=false;

    bool recording=false;

    void sendOurValues(std::vector<uint8_t> OurValues);

    bool virtuaModeActive = false;
    std::string inputCameraPath = "v4l2:/dev/video1";

    ControlRecord recordData;

    Simulation simSim;
    ClientSender virtualSender;

    int countOfSticks = 0;
    int outputType = 0;
    bool autopilotMode = false;
    stack<Message> autopilotStack;

    bool setOutputType(int i);


    void drawDrone();

    /** To be deleted ASAP **/
    void drawDzhanibekov();
    void drawTestObject();

};

#endif // PHYSICSMAINWINDOW_H
