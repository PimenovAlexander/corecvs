#ifndef JOYSTICKINPUT_H
#define JOYSTICKINPUT_H
//#include "controllrecord.h"

#include <linux/joystick.h>
#include <fcntl.h>
class JoyStickInput
{
public:
    JoyStickInput(int &_yawValue, int &_rollValue, int &_pitchValue, int &_throttleValue,
                  int &_CH5Value, int &_CH6Value, int &_CH7Value,  int &_CH8Value );

    struct AxisState { short x, y; };

    void start();

    int& yawValue;
    int& rollValue;
    int& pitchValue;
    int& throttleValue;
    int& CH5Value;
    int& CH6Value;
    int& CH7Value;
    int& CH8Value;
    void setUsualCurrentMode();
    void setInertiaCurrentMode();
    void setCasualCurrentMode();
    void setRTLTUsialMode();

    void fullRtSticks(js_event event);
    void setRTLTFullMode();
private:
    int throttleValueFromJS;
    int midThrottle=1350;
    void startJoyStickMode();
    int countOfSticks=0;
    int currentMode=0;
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
