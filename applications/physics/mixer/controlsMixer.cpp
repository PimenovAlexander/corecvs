#include "controlsMixer.h"

bool ControlsMixer::mix(JoystickState &state, CopterInputs &outputs)
{
    if (state.axis.size() < 4) {
        return false;
    }

    /* This need to be brought to mixer */
    double d = (2 * 32767.0);

    outputs.axis[CopterInputs::CHANNEL_THROTTLE] = -(state.axis[1] / d * 100.0);
    outputs.axis[CopterInputs::CHANNEL_ROLL    ] =   state.axis[3] / d * 100.0;
    outputs.axis[CopterInputs::CHANNEL_PITCH   ] =   state.axis[4] / d * 100.0;
    outputs.axis[CopterInputs::CHANNEL_YAW     ] =   state.axis[0] / d * 100.0;

    return true;
}

ControlsMixer::ControlsMixer()
{

}
