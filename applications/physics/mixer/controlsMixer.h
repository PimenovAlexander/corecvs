#ifndef CONTROLSMIXER_H
#define CONTROLSMIXER_H

#include <copterInputs.h>
#include <joystickInterface.h>



class ControlsMixer
{
public:
    class ChannelOperation
    {


        int defaultValue = 0;
        int min;
        int max;
    };


    virtual void mix(JoystickState &state, CopterInputs &inputs);


    ControlsMixer();
};

#endif // CONTROLSMIXER_H
