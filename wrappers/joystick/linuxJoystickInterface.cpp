#include "linuxJoystickInterface.h"
#include "core/utils/utils.h"
#include "core/filesystem/folderScanner.h"

#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <thread>
#include <sstream>
#include <core/utils/preciseTimer.h>

using namespace std;
using namespace corecvs;

LinuxJoystickInterface::LinuxJoystickInterface(const string &deviceName)
{
    mDeviceName = deviceName;
    SYNC_PRINT(("LinuxJoystickInterface::LinuxJoystickInterface(%s):called\n", deviceName.c_str()));
}

vector<string> LinuxJoystickInterface::getDevices(const string &prefix)
{
    vector<string> toReturn;

    static const int maxDeviceId = 5;

    for (int i = 0; i < maxDeviceId; i ++)
    {
        /* Stupid C++99 */
        std::stringstream ss;
        ss << i;
        std::string dev = prefix + ss.str();

        if (FolderScanner::pathExists(dev))
        {
           toReturn.push_back(dev);
        }
    }
    return toReturn;
}

JoystickConfiguration LinuxJoystickInterface::getConfiguration(int joystickDevice)
{
    JoystickConfiguration toReturn;

    char axes = 0;
    if (ioctl(joystickDevice, JSIOCGAXES, &axes) != -1)
    {
        toReturn.axisNumber = axes;
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

    return toReturn;
}


JoystickConfiguration LinuxJoystickInterface::getConfiguration(const std::string &deviceName)
{
    JoystickConfiguration toReturn;

    int joystickDevice = open(deviceName.c_str(), O_RDONLY);
    if (joystickDevice == -1)
    {
        SYNC_PRINT(("Could not open joystick at <%s>", deviceName.c_str()));
        return toReturn;
    }

    toReturn = getConfiguration(joystickDevice);

    close(joystickDevice);
    return toReturn;
}

JoystickConfiguration LinuxJoystickInterface::getConfiguration()
{
    if (mJoystickDevice == -1) {
        SYNC_PRINT(("LinuxJoystickInterface::getConfiguration(): Device not open\n"));
        return JoystickConfiguration();
    }

   return getConfiguration(mJoystickDevice);
}

bool LinuxJoystickInterface::start()
{
    SYNC_PRINT(("JoystickInterface::start(): called\n"));

    if (mSpinThread != NULL) {
        SYNC_PRINT(("Already running\n"));
        return false;
    }
    mJoystickDevice = open(mDeviceName.c_str(), O_RDONLY);
    if (mJoystickDevice == -1)
    {
        SYNC_PRINT(("Failed opening device <%s>\n", mDeviceName.c_str()));
        return false;
    }
    exitLock.lock();

    mSpinThread = new std::thread(&LinuxJoystickInterface::run, this);
    return true;
}

void LinuxJoystickInterface::stop()
{
    SYNC_PRINT(("JoystickInterface::stop(): called\n"));
    if (mSpinThread == NULL)
    {
         SYNC_PRINT(("Not running\n"));
         return;
    }
    /* Ok... give thread some time to exit */

    SYNC_PRINT(("JoystickInterface::stop(): requesting the exit\n"));
    exitLock.unlock();
    SYNC_PRINT(("JoystickInterface::stop(): waiting thread to exit\n"));
    mSpinThread->join();
    delete_safe(mSpinThread);

    close(mJoystickDevice);

    SYNC_PRINT(("JoystickInterface::stop(): exiting\n"));
}

void LinuxJoystickInterface::run()
{

    struct pollfd fds[1];
    fds[0].fd = mJoystickDevice;
    fds[0].events = POLLIN;

    //struct js_event event;
    uint8_t data[sizeof(js_event)];
    int count = 0;

    /* Do we need this? */
    JoystickConfiguration conf = getConfiguration();

    JoystickState state;
    state.axis  .resize(conf.axisNumber , 0);
    state.button.resize(conf.buttonNumber, 0);

    initialTimestamp = PreciseTimer::currentTime().usec();
    SYNC_PRINT(("LinuxJoystickInterface::run(): starting time %" PRIu64 "\n", initialTimestamp));

    while (!exitLock.try_lock())
    {
        /**
         * We should not block, beacuse we should check for exitLock.
         * it is also possible to use pipe awailability as a exit condition and only use one poll for the task
         **/
        int ret = poll( fds, 1, 1);

        if (ret == 0) { /* Timeout. No new data */
            continue;
        }
        if (ret == -1) {
            SYNC_PRINT(("LinuxJoystickInterface::run(): poll() encountered an error %d\n", errno));
            continue;
        }

        size_t bytes = read(mJoystickDevice, (void *)&data[count], sizeof(js_event) - count);
        count += bytes;
        state.timestamp = PreciseTimer::currentTime().usec() - initialTimestamp;

        if (count != sizeof(js_event))
        {
            //SYNC_PRINT(("JoystickInterface::run(): Corrupted data\n"));
            continue;
        }
        count = 0;
        js_event *event = (js_event *)data;


        bool changed = false;
        if (event->type == JS_EVENT_BUTTON)
        {
            //SYNC_PRINT(("JoystickInterface::run(): button event\n"));
            state.button[event->number] = event->value;
            newButtonEvent(event->number, event->value, event->time);
            changed = true;
        }

        if (event->type == JS_EVENT_AXIS)
        {
            state.axis[event->number] = event->value;
            newAxisEvent(event->number, event->value, event->time);
            changed = true;
        }

        if (changed)
        {
            newJoystickState(state);
        }
    }
    SYNC_PRINT(("LinuxJoystickInterface::run(): Exiting spinthread....\n"));

}

void LinuxJoystickInterface::newButtonEvent(int button, int value, int timestamp)
{
    SYNC_PRINT(("JoystickInterface::newButtonEvent(%d %d %d): called\n", button, value, timestamp));
}

void LinuxJoystickInterface::newAxisEvent(int axis, int value, int timestamp)
{
    SYNC_PRINT(("JoystickInterface::newAxisEvent(%d %d %d): called\n", axis, value, timestamp));
}

void LinuxJoystickInterface::newJoystickState(JoystickState state)
{
    SYNC_PRINT(("JoystickInterface::newJoystickState(): called\n"));
}


