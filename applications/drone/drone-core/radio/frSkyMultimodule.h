#ifndef FRSKYMULTIMODULE_H
#define FRSKYMULTIMODULE_H

#include <copterInputs.h>
#include <stdint.h>

struct FrSkyMultimoduleParameters
{
    bool lowPower = false;
    int  modelId  = 0x1;
    int  subtype  = 0x0;
};

class FrSkyMultimodule : public FrSkyMultimoduleParameters
{
public:
    static const int MESSAGE_SIZE = 26;

    /** Each channel is 10 bit in FrSky **/
    static const int MASK = 0x3FF;

    static const int MAGIC_DIFFERENCE = 858;
    void pack (const CopterInputs &channels, uint8_t message[MESSAGE_SIZE]);

};

#endif // FRSKYMULTIMODULE_H
