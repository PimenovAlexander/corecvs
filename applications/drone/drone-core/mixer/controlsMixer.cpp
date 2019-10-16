#include "controlsMixer.h"

bool ControlsMixer::mix(JoystickState &state, CopterInputs &outputs)
{
    if (state.axis.size() < 4) {
        return false;
    }

    /* This need to be brought to mixer */

    usualMode(state,outputs,false);
    return true;
}

void ControlsMixer::usualMode(JoystickState &state, CopterInputs &outputs,bool xInputOn)
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


ControlsMixer::ControlsMixer()
{

}
