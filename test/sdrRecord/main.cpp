#include <fstream>

#include "sdrRecord.h"

int main (int argc, char **argv)
{
    if (argc == 0)
    {
        SYNC_PRINT(("enter centerFreq\n"));
        return 0;
    }
    double centerFreq = strtod(argv[1], nullptr);
    SYNC_PRINT(("Initializing capture on frequency %f\n", centerFreq));
    auto sdrCapture = new sdrRecord(centerFreq);
    sdrRecord::CapErrorCode errorCode;

    errorCode = sdrCapture->initCapture();
    if (errorCode == sdrRecord::CapErrorCode::FAILURE)
    {
        SYNC_PRINT(("Cannot init capture\n"));
        return 1;
    }

    errorCode = sdrCapture->startCapture();
    if (errorCode == sdrRecord::CapErrorCode::FAILURE)
    {
        SYNC_PRINT(("Cannot start capture\n"));
        return 1;
    }

    SYNC_PRINT(("Record started, press Enter to stop...\n"));
    std::string userInput;
    if (std::getline(std::cin, userInput))
        errorCode = sdrCapture->stopCapture();
    if (errorCode == sdrRecord::CapErrorCode::FAILURE)
    {
        SYNC_PRINT(("Error stopping capture\n"));
        return 1;
    }

    delete sdrCapture;
    return 0;
}

