#ifndef FRSKYMULTIMODULE_H
#define FRSKYMULTIMODULE_H

#include <stdint.h>

class FrSkyMultimodule
{
public:

    enum ChannelID {
        CHANNEL_0,
        CHANNEL_THROTTLE = CHANNEL_0,
        CHANNEL_1,
        CHANNEL_ROLL     = CHANNEL_1,
        CHANNEL_2,
        CHANNEL_PITCH    = CHANNEL_2,
        CHANNEL_3,
        CHANNEL_YAW      = CHANNEL_3,


        CHANNEL_4,
        CHANNEL_5,
        CHANNEL_6,
        CHANNEL_7,

        CHANNEL_LAST
    };

    static const int MESSAGE_SIZE = 26;

    /** Each channel is 10 bit in FrSky **/
    static const int MASK = 0x3FF;

    static const int MAGIC_DIFFERENCE = 858;
    static void pack (int16_t channels[CHANNEL_LAST], uint8_t message[MESSAGE_SIZE]);


    static void pack1(int16_t channels[CHANNEL_LAST], uint8_t message[MESSAGE_SIZE]);

};

#endif // FRSKYMULTIMODULE_H
