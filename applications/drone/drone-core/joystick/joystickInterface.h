#ifndef JOYSTICKINTERFACE_H
#define JOYSTICKINTERFACE_H

#include <mutex>
#include <string>
#include <vector>

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

    JoystickInterface(const std::string &deviceName):
        mDeviceName(deviceName)
    {}

    static std::vector<std::string> getDevices      (const std::string &prefix = "/dev/input/js");
    static JoystickConfiguration    getConfiguration(const std::string &deviceName);


    JoystickConfiguration getConfiguration();
    void start();
    void stop();
    void run();

    /**
     * Callbacks are here.
     * They are called from some thread, user may not expect anything from this thread
     * and must try to exit ASAP.
     **/
    virtual void newButtonEvent    (int button, int value, int timestamp);
    virtual void newAxisEvent      (int axis  , int value, int timestamp);
    virtual void newJoystickState(JoystickState state);


protected:
    int mJoystickDevice = -1;

    static JoystickConfiguration getConfiguration(int joystickDevice);


    std::thread *mSpinThread = NULL;
    std::timed_mutex  exitLock;

};

#endif // JOYSTICKINTERFACE_H
