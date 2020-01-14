#ifndef JOYSTICKINTERFACE_H
#define JOYSTICKINTERFACE_H

#include <vector>
#include <string>

namespace corecvs {

class JoystickConfiguration
{
public:
    std::string name;

    int axisNumber = 0;
    int buttonNumber = 0;

    int version;

    void print();
};

class JoystickState
{
public:
    /* We could also log timestamps etc*/
    uint64_t timestamp;

    std::vector<int> axis;
    std::vector<int> button;
};

class JoystickInterface
{
public:
    std::string mDeviceName;

    JoystickInterface() {}

    JoystickInterface(const std::string &deviceName):
        mDeviceName(deviceName)
    {}

    //static std::vector<std::string> getDevices      (const std::string &prefix = "/dev/input/js");
    //static JoystickConfiguration    getConfiguration(const std::string &deviceName);


    virtual JoystickConfiguration getConfiguration() = 0;
    virtual bool start() = 0;
    virtual void stop() = 0;

    /**
     * Callbacks are here.
     * They are called from some thread, user may not expect anything from this thread
     * and must try to exit ASAP.
     **/
    virtual void newButtonEvent    (int /*button*/, int /*value*/, int /*timestamp*/) {}
    virtual void newAxisEvent      (int /*axis*/  , int /*value*/, int /*timestamp*/) {}
    virtual void newJoystickState  (JoystickState /*state*/)                          {}

    virtual ~JoystickInterface() {}
};


} // namespace corecvs

#endif // JOYSTICKINTERFACE_H
