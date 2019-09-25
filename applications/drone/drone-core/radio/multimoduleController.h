#ifndef QCOMCONTROLLER_H
#define QCOMCONTROLLER_H

#include <vector>
#include <mutex>

#include "copterInputs.h"
#include "frSkyMultimodule.h"


namespace std {
    class thread;
}

class MultimoduleController
{    
public:
    explicit MultimoduleController();
    /* */
public:
    static std::vector<std::string> getDevices(const std::string &prefix = "/dev/ttyUSB");
    bool connectToModule(const std::string &device);
    bool disconnect();
    void newData(const CopterInputs &input);

    FrSkyMultimodule protocol;
    std::thread *mSpinThread = NULL;

private:

    int serialPort;
    std::mutex channelsProtect;
    CopterInputs channels;

    bool timeToStop; /* Use Mutex */
    void run();
    void sendOurValues(std::vector<uint8_t> OurValues);
};


#endif // QCOMCONTROLLER_H
