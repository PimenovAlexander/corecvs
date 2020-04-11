#include <iostream>
#include "core/utils/global.h"
#include "joystickInterface.h"

namespace corecvs {

using namespace std;

void JoystickConfiguration::print() {
    cout << "Joystick: " << name << endl;
    cout << "Version: " << version << endl;

    cout << "Axis    :" << axisNumber << endl;
    cout << "Buttons :" << buttonNumber << endl;
}

JoystickInterface::JoystickInterface()
{
    SYNC_PRINT(("JoystickInterface::JoystickInterface(): called.\n"));
}

/*
JoystickInterface::JoystickInterface(const string &deviceName):
    mDeviceName(deviceName)
{
    SYNC_PRINT(("JoystickInterface::JoystickInterface(string): called. Setting device to <%s>\n", mDeviceName.c_str()));
}
*/

} // namespace corecvs
