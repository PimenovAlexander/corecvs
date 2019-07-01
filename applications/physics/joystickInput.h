#ifndef JOYSTICKINPUT_H
#define JOYSTICKINPUT_H
//#include "controllrecord.h"

#include <linux/joystick.h>
#include <fcntl.h>
#include "copterInputs.h"

class JoyStickInput
{
public:
    JoyStickInput( );

    struct AxisState { short x, y; };

    CopterInputs output;                   //input , output, who cares?

    void start();
    bool active=false;
    bool mutexActive=false;
    int yawValue=1500;
    int rollValue=1500;
    int pitchValue=1500;
    int throttleValue=1500;
    int CH5Value=1500;
    int CH6Value=1500;
    int CH7Value=1500;
    int CH8Value=1500;
    void setUsualCurrentMode();
    void setInertiaCurrentMode();
    void setCasualCurrentMode();
    void setRTLTUsialMode();

    void fullRtSticks(js_event event);
    void setRTLTFullMode();
    void updateOutput();
    void extreamRtSticks(js_event event);
    void setRTLTExtreamMode();
private:
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

    void timerForThrottle();
    void usualExperimentalSticks(js_event event);
    void usualExperimentalButtons(js_event event);
};

#endif // JOYSTICKINPUT_H
