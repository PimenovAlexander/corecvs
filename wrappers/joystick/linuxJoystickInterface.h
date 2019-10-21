#ifndef LINUX_JOYSTICKINTERFACE_H
#define LINUX_JOYSTICKINTERFACE_H

#include <vector>
#include <string>
#include <mutex>

#include <core/joystick/joystickInterface.h>

namespace std {
    class thread;
}

class LinuxJoystickInterface
{
public:
    std::string mDeviceName;

    LinuxJoystickInterface(const std::string &deviceName):
        mDeviceName(deviceName)
    {}

    static std::vector<std::string>         getDevices      (const std::string &prefix = "/dev/input/js");
    static corecvs::JoystickConfiguration   getConfiguration(const std::string &deviceName);


    corecvs::JoystickConfiguration getConfiguration();
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
    virtual void newJoystickState(corecvs::JoystickState state);


protected:
    int mJoystickDevice = -1;

    static corecvs::JoystickConfiguration getConfiguration(int joystickDevice);


    std::thread *mSpinThread = NULL;
    std::timed_mutex  exitLock;

};


#endif // LINUX_JOYSTICKINTERFACE_H
