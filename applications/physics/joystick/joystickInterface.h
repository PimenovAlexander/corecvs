#ifndef JOYSTICKINTERFACE_H
#define JOYSTICKINTERFACE_H

#include <string>
#include <vector>

class JoystickConfiguration
{
public:
    std::string name;

    int stickNumber = 0;
    int buttonNumber = 0;

    int version;

    void print();


};

class JoystickState
{
public:
    /* We could also log timestamps etc*/

    std::vector<int> axis;
    std::vector<int> button;

};

class JoystickStateReciever
{

};

namespace std {
    class thread;
}

class JoystickInterface
{
public:
    std::string mDeviceName;

    JoystickInterface(std::string deviceName):
        mDeviceName(deviceName)
    {}

    static std::vector<std::string> getDevices      (const std::string &prefix = "/dev/input/js");
    static JoystickConfiguration    getConfiguration(const std::string &deviceName);

    void start();
    void stop();

    void run();

    /**
     * Callbacks are here.
     * They are called from some thread, user may not expect anything from this thread
     * and must try to exit ASAP.
     **/
    virtual void newButtonEvent();
    virtual void newAxisEvent();
    virtual void newJoystickState();

private:
    std::thread *spinThread = NULL;

};

#endif // JOYSTICKINTERFACE_H
