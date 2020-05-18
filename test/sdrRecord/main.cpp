#include "core/reflection/commandLineSetter.h"
#include "core/utils/utils.h"

#include <fstream>

#include "sdrRecord.h"

using namespace corecvs;

int main (int argC, char **argV)
{
    CommandLineSetter s(argC, argV);

    if (s.hasOption("list"))
    {
        SDRRecord::printDevices();
        return 0;
    }

    vector<string> input = s.nonPrefix();

    if (input.size() != 2)
    {
        SYNC_PRINT(("Usage:\n"));
        SYNC_PRINT(("  sdrRecord <centerFreq>\n"));
        SYNC_PRINT(("    - Will capture quadratures from <centerFreq>\n"));

        SYNC_PRINT(("You have provided %d argumants\n", argC));

        return 0;
    }

    double centerFreq = HelperUtils::parseDouble(input[1]);

    SYNC_PRINT(("Initializing capture on frequency %f Hz (%f MHz)\n", centerFreq, centerFreq / 1000000.0));

    SDRRecord* sdrCapture = new SDRRecord(centerFreq);
    SDRRecord::CapErrorCode errorCode;

    errorCode = sdrCapture->initCapture();
    if (errorCode == SDRRecord::CapErrorCode::FAILURE)
    {
        SYNC_PRINT(("Cannot init capture\n"));
        return 1;
    }

    errorCode = sdrCapture->startCapture();
    if (errorCode == SDRRecord::CapErrorCode::FAILURE)
    {
        SYNC_PRINT(("Cannot start capture\n"));
        return 1;
    }

    SYNC_PRINT(("Record started, press Enter to stop...\n"));
    std::string userInput;
    if (std::getline(std::cin, userInput))
        errorCode = sdrCapture->stopCapture();
    if (errorCode == SDRRecord::CapErrorCode::FAILURE)
    {
        SYNC_PRINT(("Error stopping capture\n"));
        return 1;
    }

    delete sdrCapture;
    return 0;
}

