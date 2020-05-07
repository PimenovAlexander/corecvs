/**
 * \file ATVCapture.cpp
 * \brief Analogue TV decoder for SDR
 *
 * \date May 7, 2020
 * \author Ilya
 */

#ifndef CORECVS_SDRRECORD_H
#define CORECVS_SDRRECORD_H

#include <cstdlib>
#include <complex>
#include <algorithm>
#include <fstream>
#include <iostream>

#include <thread>
#include <mutex>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

#include <core/utils/global.h>


class sdrRecord
{
public:
    explicit sdrRecord(double centerFreq);

    enum CapErrorCode
    {
        SUCCESS = 0,
        FAILURE = 2
    };

    sdrRecord::CapErrorCode initCapture();
    sdrRecord::CapErrorCode startCapture();
    sdrRecord::CapErrorCode stopCapture();

    ~sdrRecord();
    bool mIsPaused;
    bool firstIteration;

private:
    constexpr static const double sampleRate = 10e6;
    const double centerFreq;
    static const short buffSize = 1024;
    std::ofstream file;

    SoapySDR::Device* SDR;
    SoapySDR::Stream* rxStream;

    void receiving(std::complex<float> buffer[]);
    void writing();

    std::thread receiver, writer;
    std::complex<float> buff[2][buffSize];
};

#endif //CORECVS_SDRRECORD_H
