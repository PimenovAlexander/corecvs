#ifndef PLAYBACK_JOYSTICK_INTERFACE_H
#define PLAYBACK_JOYSTICK_INTERFACE_H

#include <mutex>
#include <string>
#include <thread>

#include "joystickInterface.h"


namespace corecvs {

class JoystickFileFormat
{
 public:
    JoystickConfiguration configuration;
    std::vector<JoystickState> states;

    JoystickFileFormat() {}

    void addState(JoystickState &state)
    {
        states.push_back(state);
    }

    bool save(std::ostream &file);
    bool load(std::istream &file);

    bool save(const std::string &fileName);
    bool load(const std::string &fileName);

    void reset(const JoystickConfiguration &configuration);

};


class PlaybackJoystickInterface : public JoystickInterface
{
public:
    PlaybackJoystickInterface(const std::string &deviceName):
        corecvs::JoystickInterface(deviceName)
    {}

    static corecvs::JoystickConfiguration  getConfiguration(const std::string &deviceName);

    corecvs::JoystickConfiguration getConfiguration();
    virtual bool start() override;
    virtual void stop()  override;

    void run ();
    /**
     * Callbacks are here.
     * They are called from some thread, user may not expect anything from this thread
     * and must try to exit ASAP.
     **/
    virtual void newButtonEvent    (int button, int value, int timestamp)  override;
    virtual void newAxisEvent      (int axis  , int value, int timestamp)  override;
    virtual void newJoystickState  (corecvs::JoystickState state)          override;


protected:
    std::thread *mSpinThread = NULL;
    std::timed_mutex  exitLock;

};


} // namespace corecvs

#endif // PLAYBACK_JOYSTICK_INTERFACE_H
