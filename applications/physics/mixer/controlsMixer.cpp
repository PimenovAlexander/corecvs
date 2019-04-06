#include "controlsMixer.h"

bool ControlsMixer::mix(JoystickState &state, CopterInputs &outputs)
{
    if (state.axis.size() < 4) {
        return false;
    }

    /* This need to be brought to mixer */

    switch (state.axis.size())
    {
        case 4:
        switch (currentMode)
        {
        case 0:
            usialMode(state,outputs,false);
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        }
            break;
        case 6:
        switch (currentMode)
        {
        case 0:
            usialMode(state,outputs,true);
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;

        }
            break;
    }


    return true;
}

void ControlsMixer::usialMode(JoystickState &state, CopterInputs &outputs,bool xInputOn)
{
    double d = (2 * 32767.0);
    if (xInputOn)
    {
        outputs.axis[CopterInputs::CHANNEL_THROTTLE] = -(state.axis[1] / d * 100.0);
        outputs.axis[CopterInputs::CHANNEL_ROLL    ] =   state.axis[3] / d * 100.0;
        outputs.axis[CopterInputs::CHANNEL_PITCH   ] =   state.axis[4] / d * 100.0;
        outputs.axis[CopterInputs::CHANNEL_YAW     ] =   state.axis[0] / d * 100.0;
    }
    else
    {
        outputs.axis[CopterInputs::CHANNEL_THROTTLE] = -(state.axis[1] / d * 100.0);
        outputs.axis[CopterInputs::CHANNEL_ROLL    ] =   state.axis[3] / d * 100.0;
        outputs.axis[CopterInputs::CHANNEL_PITCH   ] =   state.axis[4] / d * 100.0;
        outputs.axis[CopterInputs::CHANNEL_YAW     ] =   state.axis[0] / d * 100.0;
    }
}

/*
void JoyStickInput::usualButtons(js_event event)
{
    unsigned char seven=7;
    unsigned char six=6;
    unsigned char five=5;
    unsigned char four=4;
    unsigned char three=3;
    unsigned char two=2;
    unsigned char one=1;
    unsigned char zero=0;
    switch (countOfSticks)
    {

    case 6:                            //Dinput
        if (event.number==seven)                     //arming    //rt
        {
            startArming(event.value);
        }
        if (event.number==five  && event.value )      //rb         //turn of copter(if smth goes wery wery wrong)
        {
            disconnectFromCopter();
        }
        if (event.number==one && event.value)         //a
        {
            if (!recording)
            {
                startRecord();
            }
            else
            {
                stopRecord();
            }
        }
        break;                          //Xinput
    case 8:

        if (event.number==five  && event.value )      //rb         //turn of copter(if smth goes wery wery wrong)
        {
            disconnectFromCopter();
        }
        if (event.number==zero && event.value)         //a
        {
            if (!recording)
            {
                startRecord();
            }
            else
            {
                stopRecord();
            }
        }
        break;
    }
}
*/
ControlsMixer::ControlsMixer()
{

}
