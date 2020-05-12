/**
 * \file ATVCapture.cpp
 * \brief Analogue TV decoder for SDR
 *
 * \date Mar 13, 2020
 * \author Ilya
 */

#ifndef CORECVS_ATV_H_
#define CORECVS_ATV_H_

#include <cstdlib>
#include <complex>
#include <vector>
#include <algorithm>
#include <fftw3.h>

#include <thread>
#include <mutex>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

#include "core/framesources/cameraControlParameters.h"
#include "core/framesources/imageCaptureInterface.h"
#include "filter.h"


class ATVCapture : public virtual ImageCaptureInterface
{
public:
    ATVCapture(double channelFreq, bool isRGB);
    ATVCapture(const std::string &channel, bool isRGB);

    ImageCaptureInterface::CapErrorCode initCapture()  override;
    ImageCaptureInterface::CapErrorCode startCapture() override;
    ImageCaptureInterface::CapErrorCode pauseCapture() override;
    ImageCaptureInterface::FramePair    getFrame()     override;

    ~ATVCapture() override;

    bool supportPause() override;
    bool mIsPaused;
    bool mIsRGB;

    CameraFormat cameraFormat = CameraFormat(525, 700, 30);

private:
    constexpr static const double sampleRate = 10e6;
    const double centerFreq;
    static const short buffSize = 1024;

    SoapySDR::Device* SDR = NULL;
    SoapySDR::Stream* rxStream = NULL;

    void receiving();
    void filtering();
    void decoding();

    std::thread receiver, filter, decoder;
    std::complex<float> buff[4][buffSize];
    std::mutex bufferMutex[4];

    // double buffering with page flipping
    G12Buffer   *lumBackFrame, *lumFrontFrame;
    RGB24Buffer *backFrame, *frontFrame;

    // buffers for FFTW
    fftwf_complex *input, *output, *freqLum, *freqColor, *lum, *color;

    fftwf_plan forwardTransform, backwardLumTransform, backwardColorTransform;

    static double getChannelFreq(std::string channel);

    std::vector<float> debug = {0};

    // ImageCaptureInterface interface
public:
    int gain = 0;
    int brightness = 0;
    int contrast = 0;

    virtual CapErrorCode queryCameraParameters(CameraParameters &parameter) override;
    virtual CapErrorCode setCaptureProperty(int id, int value) override;
    virtual CapErrorCode getCaptureProperty(int id, int *value) override;
};


class ATVCaptureProducer : public ImageCaptureInterfaceProducer
{
public:
    ATVCaptureProducer() = default;

    std::string getPrefix() override
    {
        return "atv:";
    }

    ImageCaptureInterface* produce(std::string &name, bool isRGB) override
    {
        return new ATVCapture(name, isRGB);
    }
};

#define HORIZONTAL_SYNC_DURATION 4.7 * 0.000001
#define BACK_PORCH_DURATION 4.7  * 0.000001
#define VIDEO_DURATION 52.6  * 0.000001
#define FRONT_PORCH_DURATION 1.5 * 0.000001
#define LINE_DURATION 63.5 * 0.000001

#define IDLE 1
#define LINES_TRANSMISSION 2
#define FRONT_PORCH 3
#define HORIZONTAL_SYNC 4
#define BACK_PORCH 5
#define VIDEO 6
#define VERTICAL_SYNC 7
#define EQUALISING 8
#define SERRATION 9
#define BLANKING 10
#define EVEN 1
#define ODD 0

#define BLACK_LEVEL 0
#define WHITE_LEVEL 0
#define HORIZONTAL_SYNC_THRESHOLD 0
#define HORIZONTAL_SYNC_LEVEL 0
#define BACK_PORCH_LEVEL 0
#define FRONT_PORCH_LEVEL 0
#define EQUALISING_LEVEL 0
#define VERTICAL_SYNC_LEVEL 0

#endif /* CORECVS_ATV_H_ */
