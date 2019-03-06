#include "joystickInterface.h"

#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>

#include "core/utils/global.h"
#include "core/utils/utils.h"

using namespace std;


vector<string> JoystickInterface::getDevices(const string &prefix)
{
    vector<string> toReturn;

    static const int maxDeviceId = 5;

    for (int i = 0; i < maxDeviceId; i ++)
    {
        /* Stupid C++99 */
        std::stringstream ss;
        ss << i;
        std::string dev = prefix + ss.str();

        if (corecvs::HelperUtils::pathExists(dev))
        {
           toReturn.push_back(dev);
        }
    }
    return toReturn;

}

JoystickConfiguration JoystickInterface::getConfiguration(const std::string &deviceName)
{
    JoystickConfiguration toReturn;

    int joystickDevice = open(deviceName.c_str(), O_RDONLY);
    if (joystickDevice == -1)
    {
        SYNC_PRINT(("Could not open joystick at <%s>", deviceName.c_str()));
        return toReturn;
    }

    char axes = 0;
    if (ioctl(joystickDevice, JSIOCGAXES, &axes) != -1)
    {
        toReturn.stickNumber = axes;
    }

    char buttons = 0;
    if (ioctl(joystickDevice, JSIOCGBUTTONS, &buttons) != -1)
    {
        toReturn.buttonNumber = buttons;
    }


    int version = 0;
    if (ioctl(joystickDevice, JSIOCGVERSION, &version) != -1)
    {
        toReturn.version = version;
    }

    char name[128] = "Unknown";
    if (ioctl(joystickDevice, JSIOCGNAME(sizeof(name)), name) != -1)
    {
        toReturn.name = name;
    }

    close(joystickDevice);
    return toReturn;
}

void JoystickInterface::start()
{
    //spinThread = new std::thread();

}

void JoystickInterface::stop()
{

}

void JoystickInterface::run()
{
#if 0
    size_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
    {


    }
#endif
}

void JoystickInterface::newButtonEvent()
{
    SYNC_PRINT(("JoystickInterface::newButtonEvent(): called\n"));
}

void JoystickInterface::newAxisEvent()
{
    SYNC_PRINT(("JoystickInterface::newAxisEvent(): called\n"));
}

void JoystickInterface::newJoystickState()
{
    SYNC_PRINT(("JoystickInterface::newJoystickState(): called\n"));
}

void JoystickConfiguration::print() {
    cout << "Joystick: " << name << endl;
    cout << "Version: " << version << endl;

    cout << "Axis" << stickNumber << endl;
    cout << "Buttons" << stickNumber << endl;
}
