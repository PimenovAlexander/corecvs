#ifndef JOYSTICKINPUT_H
#define JOYSTICKINPUT_H
//#include "controllrecord.h"

#include <linux/joystick.h>
#include <fcntl.h>

class JoyStickInput
{
public:
     JoyStickInput(int &_yaw_value, int &_roll_value, int &_pitch_value, int &_throttle_value, int &_CH5_value, int &_CH6_value, int &_CH7_value,  int &_CH8_value );

    struct axis_state {       short x, y;    };

    void Start();

    int& yaw_value;
    int& roll_value;
    int& pitch_value;
    int& throttle_value;
    int& CH5_value;
    int& CH6_value;
    int& CH7_value;
    int& CH8_value;
    void SetUsualCurrentMode();
    void SetInertiaCurrentMode();
    void SetCasualCurrentMode();
    void SetRTLTUsialMode();

    void full_rt_sticks(js_event event);
    void SetRTLTFullMode();
private:
    int throttle_value_from_JS;
    int mid_Throttle=1350;
    void StartJoyStickMode();
    int CountOfSticks=0;
    int Current_mode=0;
    bool autopilotMode=false;
    bool rt_pressed=false;
    bool lt_pressed=false;
    bool recording=false;
    bool Arming=false;

    struct axis_state axes[3] ;


    void usual_buttons(js_event event);
    void usual_sticks(js_event event);
    void inertial_buttons(js_event event);
    void inertial_sticks(js_event event);
    void casual_buttons(js_event event);
    void casual_sticks(js_event event);
    void Start_arming(bool pressed);
    void disconnect_from_copter();
    int sign(int val);
    void StartRecord();
    void StopRecord();

    //ControllRecord recordData;

    int lastLT=-32767;
    int lastRT=-32767;

    void TimerForThrottle();
    void usial_experimental_sticks(js_event event);
    void usial_experimental_buttons(js_event event);
};

#endif // JOYSTICKINPUT_H
