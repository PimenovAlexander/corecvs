/**
 * \file ATVCapture.cpp
 * \brief Analogue TV decoder for SDR
 *
 * \date Mar 13, 2020
 * \author Ilya
 */

#include "sdrRecord.h"

SDRRecord::SDRRecord(double centerFreq) : centerFreq(centerFreq), mIsPaused(true) {}


SDRRecord::CapErrorCode SDRRecord::initCapture()
{
    SYNC_PRINT(("sdrRecord::initCapture(): called\n"));

    // TODO: automatic choice of device
    SoapySDR::Kwargs args = SoapySDR::KwargsFromString("driver=hackrf");
    SDR = SoapySDR::Device::make(args);
    if (SDR == nullptr)
    {
        SYNC_PRINT(("sdrRecord::initCapture(): Unable to make a device\n"));
        return SDRRecord::FAILURE;
    }

    /* Output some information */
    std::string driverKey   = SDR->getDriverKey();
    std::string hardwareKey = SDR->getHardwareKey();

    SYNC_PRINT(("Driver   Key: %s\n", driverKey.c_str()));
    SYNC_PRINT(("Hardware Key: %s\n", hardwareKey.c_str()));

    SYNC_PRINT(("Hardware Info:\n"));
    SoapySDR::Kwargs list = SDR->getHardwareInfo();
    for (auto &key : list)
    {
        SYNC_PRINT(("    %s - %s:\n", key.first.c_str(), key.second.c_str()));
    }

    SYNC_PRINT(("Stream formats:\n"));
    std::vector<std::string> streamFormats = SDR->getStreamFormats(SOAPY_SDR_RX, 0);
    for (auto &format : streamFormats)
    {
        SYNC_PRINT((" - %s:\n", format.c_str()));
    }

    SYNC_PRINT(("Native formats:\n"));
    double fullScale = 0.0;
    std::string nativeStreamFormat = SDR->getNativeStreamFormat(SOAPY_SDR_RX, 0, fullScale);
    SYNC_PRINT((" - %s:\n", nativeStreamFormat.c_str()));


    /* Creating and opening stream */
    SDR->setSampleRate(SOAPY_SDR_RX, 0, sampleRate);
    SDR->setFrequency (SOAPY_SDR_RX, 0, centerFreq);
    rxStream = SDR->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CF32);

    /* Output stream info */
    size_t mtu = SDR->getStreamMTU(rxStream);
    SYNC_PRINT(("Created stream MTU is %d elements\n", (int)mtu));

    size_t dmaBuffs = SDR->getNumDirectAccessBuffers(rxStream);
    SYNC_PRINT(("Created stream has %d DMA buffers\n", (int)dmaBuffs));


    std::string output = "samples.bin";
    file.open(output, std::ios::out | std::ios::binary);
    if (file.fail())
    {
        SYNC_PRINT(("Can't open output file <%s>\n", output.c_str()));
        return SDRRecord::FAILURE;
    }

    SYNC_PRINT(("sdrRecord::initCapture(): exited\n"));
    return SDRRecord::SUCCESS;
}

SDRRecord::CapErrorCode SDRRecord::startCapture()
{
    SYNC_PRINT(("sdrRecord::startCapture(): called\n"));

    SDR->activateStream(rxStream, 0, 0, 0);
    mIsPaused = false;
    firstIteration = true;

    writer = std::thread(&SDRRecord::writing, this);

    SYNC_PRINT(("sdrRecord::startCapture(): exited\n"));
    return SDRRecord::SUCCESS;
}


void SDRRecord::receiving(std::complex<float> buffer[])
{
    void *buffs[] = {buffer};
    int flags;
    long long time_ns;
    SDR->readStream(rxStream, buffs, buffSize, flags, time_ns, 1e5);
}


void SDRRecord::writing()
{
    float re, im;
    for (u_char i = 0; !mIsPaused; i = 1 - i)
    {
        receiver = std::thread(&SDRRecord::receiving, this, buff[i]);
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


SDRRecord::CapErrorCode SDRRecord::stopCapture()
{
    SYNC_PRINT(("sdrRecord::stopCapture(): called.\n"));

    mIsPaused = true;
    writer.join();
    SDR->deactivateStream(rxStream, 0, 0);
    return SDRRecord::SUCCESS;
}


SDRRecord::~SDRRecord()
{
    SYNC_PRINT(("sdrRecord::sdrRecord(): called\n"));

    if (!mIsPaused)
        stopCapture();
    file.close();
    SDR->closeStream(rxStream);
    SoapySDR::Device::unmake(SDR);

    SYNC_PRINT(("sdrRecord::sdrRecord(): exited\n"));
}

void SDRRecord::printDevices()
{
    SoapySDR::KwargsList list = SoapySDR::Device::enumerate();
    SYNC_PRINT(("SoapySDR returns following devices:\n"));
    int count = 1;
    for (auto &dev : list)
    {
        SYNC_PRINT(("  Device %d:\n", count));

        for (auto &key : dev)
        {
            SYNC_PRINT(("    %s - %s:\n", key.first.c_str(), key.second.c_str()));
        }
        count++;
    }
}
