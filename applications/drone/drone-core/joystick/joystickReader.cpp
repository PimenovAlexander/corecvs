#include "core/utils/global.h"

#include "joystickReader.h"
#include "iostream"

JoystickReader::JoystickReader(const std::string &deviceName) :
    LinuxJoystickInterface (deviceName)
{
    SYNC_PRINT(("JoystickReader::JoystickReader(%s): called\n", deviceName.c_str()));
}


JoystickReader::~JoystickReader()
{

}
