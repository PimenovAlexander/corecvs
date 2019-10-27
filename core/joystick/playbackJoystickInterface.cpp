#include <fstream>

#include "core/utils/global.h"
#include "core/utils/utils.h"
#include "core/joystick/playbackJoystickInterface.h"

using namespace corecvs;
using namespace std;

bool PlaybackJoystickInterface::start()
{
    SYNC_PRINT(("PlaybackJoystickInterface::start(): called\n"));

    if (mSpinThread != NULL) {
        SYNC_PRINT(("Already running\n"));
        return false;
    }

    exitLock.lock();
    mSpinThread = new std::thread(&PlaybackJoystickInterface::run, this);
    return true;
}


void PlaybackJoystickInterface::stop()
{
    SYNC_PRINT(("PlaybackJoystickInterface::stop(): called\n"));
    if (mSpinThread == NULL)
    {
         SYNC_PRINT(("Not running\n"));
         return;
    }
    /* Ok... give thread some time to exit */

    SYNC_PRINT(("PlaybackJoystickInterface::stop(): requesting the exit\n"));
    exitLock.unlock();
    SYNC_PRINT(("PlaybackJoystickInterface::stop(): waiting thread to exit\n"));
    mSpinThread->join();
    delete_safe(mSpinThread);

    SYNC_PRINT(("PlaybackJoystickInterface::stop(): exiting\n"));
}

void PlaybackJoystickInterface::run()
{
    while (!exitLock.try_lock())
    {



    }

}

/***
    std::string name;

    int axisNumber = 0;
    int buttonNumber = 0;

    int version;
 **/

bool JoystickFileFormat::save(std::ostream &file)
{
    file << configuration.axisNumber << " " << configuration.buttonNumber << std::endl;

    if (!states.empty())
    {
        for (size_t i = 0; i < states.size(); i++)
        {
            file << states[i].timestamp << " ";
            for (size_t a = 0; a < states[i].axis.size(); a++)
            {
                if (i == 0 || states[i].axis[a] != states[i-1].axis[a])
                    file << a << " " << states[i].axis[a] << " ";
            }

            for (size_t b = 0; b < states[i].button.size(); b++)
            {
                if (i == 0 || states[i].button[b] != states[i-1].button[b])
                    file << (b + states[i].axis.size()) << " " << states[i].button[b] << " ";
            }

            file << std::endl;
        }
    }
    return true;
}

bool JoystickFileFormat::load(std::istream &file)
{
    file >> configuration.axisNumber;
    file >> configuration.buttonNumber;


    JoystickState state;

    state.axis  .resize(configuration.axisNumber, 0);
    state.button.resize(configuration.buttonNumber, 0);

    while (file)
    {
        std::string line;
        HelperUtils::getlineSafe (file, line);
        std::istringstream work(line);

        work >> state.timestamp;
        if (work) {
            cout << "Processing timestamp: " <<  state.timestamp << endl;
        }

        bool changed = false;
        while (work)
        {
            int id = 0;
            int value = 0;

            work >> id;
            work >> value;

            cout << "id: " << id << " value: " << value << endl;

            if      (id < configuration.axisNumber)
            {
                state.axis[id] = value;
                changed = true;
            }
            else if (id < configuration.axisNumber + configuration.buttonNumber)
            {
                state.button[id - configuration.axisNumber] = value;
                changed = true;
            }
        }
        if (changed) {
            states.push_back(state);
        }
    }
    return true;
}

bool JoystickFileFormat::save(const std::string &fileName)
{
    std::ofstream file(fileName);
    save(file);
    file.close();
    return true;
}

bool JoystickFileFormat::load(const std::string &fileName)
{
    std::ifstream file(fileName);
    load(file);
    file.close();
    return true;
}

void JoystickFileFormat::reset(const JoystickConfiguration &configuration)
{
    this->states.clear();
    this->configuration = configuration;
}
