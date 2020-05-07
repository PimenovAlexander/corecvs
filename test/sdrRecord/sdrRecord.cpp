/**
 * \file ATVCapture.cpp
 * \brief Analogue TV decoder for SDR
 *
 * \date Mar 13, 2020
 * \author Ilya
 */

#include "sdrRecord.h"

sdrRecord::sdrRecord(double centerFreq) : centerFreq(centerFreq), mIsPaused(true) {}


sdrRecord::CapErrorCode sdrRecord::initCapture()
{
    SYNC_PRINT(("sdrRecord::initCapture(): called\n"));

    // TODO: automatic chose of device
    SoapySDR::Kwargs args = SoapySDR::KwargsFromString("driver=hackrf");
    SDR = SoapySDR::Device::make(args);
    if (SDR == nullptr)
    {
        SYNC_PRINT(("sdrRecord::initCapture(): Unable to make a device\n"));
        return sdrRecord::FAILURE;
    }

    SDR->setSampleRate(SOAPY_SDR_RX, 0, sampleRate);
    SDR->setFrequency (SOAPY_SDR_RX, 0, centerFreq);
    rxStream = SDR->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CF32);

    std::string output = "samples.bin";
    file.open(output, std::ios::out | std::ios::binary);
    if (file.fail())
    {
        SYNC_PRINT(("Can't open output file <%s>\n", output.c_str()));
        return sdrRecord::FAILURE;
    }

    SYNC_PRINT(("sdrRecord::initCapture(): exited\n"));
    return sdrRecord::SUCCESS;
}

sdrRecord::CapErrorCode sdrRecord::startCapture()
{
    SYNC_PRINT(("sdrRecord::startCapture(): called\n"));

    SDR->activateStream(rxStream, 0, 0, 0);
    mIsPaused = false;
    firstIteration = true;

    writer = std::thread(&sdrRecord::writing, this);

    SYNC_PRINT(("sdrRecord::startCapture(): exited\n"));
    return sdrRecord::SUCCESS;
}


void sdrRecord::receiving(std::complex<float> buffer[])
{
    void *buffs[] = {buffer};
    int flags;
    long long time_ns;
    SDR->readStream(rxStream, buffs, buffSize, flags, time_ns, 1e5);
}


void sdrRecord::writing()
{
    float re, im;
    for (u_char i = 0; !mIsPaused; i = 1 - i)
    {
        receiver = std::thread(&sdrRecord::receiving, this, buff[i]);
        if (firstIteration)
        {   firstIteration = false; receiver.join(); continue;   }
        for (u_short sampleNumber = 0; sampleNumber < buffSize; sampleNumber++)
        {
            re = buff[1 - i][sampleNumber].real();
            im = buff[1 - i][sampleNumber].imag();
            file.write((char *)&re, sizeof (float));
            file.write((char *)&im, sizeof (float));
        }
        receiver.join();
    }
}


sdrRecord::CapErrorCode sdrRecord::stopCapture()
{
    SYNC_PRINT(("sdrRecord::stopCapture(): called.\n"));

    mIsPaused = true;
    writer.join();
    SDR->deactivateStream(rxStream, 0, 0);
    return sdrRecord::SUCCESS;
}


sdrRecord::~sdrRecord()
{
    SYNC_PRINT(("sdrRecord::sdrRecord(): called\n"));

    if (!mIsPaused)
        stopCapture();
    file.close();
    SDR->closeStream(rxStream);
    SoapySDR::Device::unmake(SDR);

    SYNC_PRINT(("sdrRecord::sdrRecord(): exited\n"));
}