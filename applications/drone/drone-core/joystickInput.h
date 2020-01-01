#ifndef JOYSTICKINPUT_H
#define JOYSTICKINPUT_H
//#include "controllrecord.h"

//#include <linux/joystick.h>
#include <fcntl.h>
#include "copterInputs.h"
#include <mutex>
#include <QObject>

class JoyStickInput  : public QObject
{
Q_OBJECT
public:
    JoyStickInput();
    virtual ~JoyStickInput() {}

    struct AxisState
    {
        short x;
        short y;
    };
    CopterInputs getOutput();

    void start();
    bool active=false;

    void setUsualCurrentMode();
    void setInertiaCurrentMode();
    void setCasualCurrentMode();
    void setRTLTUsialMode();

    void  setThrottle(int *throttle);

    void fullRtSticks(js_event event);
    void setRTLTFullMode();
    void updateOutput();
    void extreamRtSticks(js_event event);
    void setRTLTExtreamMode();

    std::mutex stopEventMutex;
    bool getStopEventStatus();
    void setStopEventStatus(bool b);
private:
    bool stopEvent = false;   //any key press will return control to the JS

    CopterInputs output;
    std::mutex outputMutex;
    std::mutex setThrottleMutex;
    int throttleValueFromJS;
    int midThrottle=1350;
    void startJoyStickMode();
    int countOfSticks=0;
    int currentMode=4;
    bool autopilotMode=false;
    bool rtPressed=false;
    bool ltPressed=false;
    bool recording=false;
    bool arming=false;
    int yawValue=1500;
    int rollValue=1500;
    int pitchValue=1500;
    int throttleValue=1500;
    int CH5Value=1500;
    int CH6Value=1500;
    int CH7Value=1500;
    int CH8Value=1500;

    struct AxisState axes[3] ;


    void usualButtons(js_event event);
    void usualSticks(js_event event);
    void inertialButtons(js_event event);
    void inertialSticks(js_event event);
    void casualButtons(js_event event);
    void casualSticks(js_event event);
    void startArming(bool pressed);
    void disconnectFromCopter();
    int  sign(int val);
    void startRecord();
    void stopRecord();

    //ControllRecord recordData;

    int lastLT=-32767;
    int lastRT=-32767;


    void usualExperimentalSticks(js_event event);
    void usualExperimentalButtons(js_event event);
    void setLtRtValues(JoyStickInput::AxisState *axes);

    void showAllSticks(AxisState axes[]);
    void countThrottle(int divider);
private slots:
    void timerForThrottle();
};

#endif // JOYSTICKINPUT_H
