#ifndef LINUX_JOYSTICKINTERFACE_H
#define LINUX_JOYSTICKINTERFACE_H

#include <vector>
#include <string>
#include <mutex>

#include <core/joystick/joystickInterface.h>

namespace std {
    class thread;
}

class LinuxJoystickInterface : public virtual corecvs::JoystickInterface
{
public:
    LinuxJoystickInterface(const std::string &deviceName):
        corecvs::JoystickInterface(deviceName)
    {}

    static std::vector<std::string>         getDevices      (const std::string &prefix = "/dev/input/js");
    static corecvs::JoystickConfiguration   getConfiguration(const std::string &deviceName);


    corecvs::JoystickConfiguration getConfiguration();
    virtual bool start() override;
    virtual void stop()  override;

    void run(void);
    /**
     * Callbacks are here.
     * They are called from some thread, user may not expect anything from this thread
     * and must try to exit ASAP.
     **/
    virtual void newButtonEvent    (int button, int value, int timestamp)  override;
    virtual void newAxisEvent      (int axis  , int value, int timestamp)  override;
    virtual void newJoystickState  (corecvs::JoystickState state)          override;

    ~LinuxJoystickInterface() {}

protected:
    /**
     * @brief initialTimestamp timestamp of capture start in microseconds (us)
     **/
    uint64_t initialTimestamp;

    int mJoystickDevice = -1;

    static corecvs::JoystickConfiguration getConfiguration(int joystickDevice);


    std::thread *mSpinThread = NULL;
    std::timed_mutex  exitLock;

};



#endif // LINUX_JOYSTICKINTERFACE_H
