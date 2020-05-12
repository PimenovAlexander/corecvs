/**
 * \file ATVCapture.cpp
 * \brief Analogue TV decoder for SDR
 *
 * \date Mar 13, 2020
 * \author Ilya
 */

#include "atvCapture.h"


u_short next_4(u_short i)
{
    return (i == 3) ? 0 : (i + 1);
}


ATVCapture::ATVCapture(const std::string& channel, bool isRGB) :
        ATVCapture(getChannelFreq(channel), isRGB) {}


ATVCapture::ATVCapture(double channelFreq, bool isRGB) :
        mIsPaused(true), mIsRGB(isRGB), centerFreq(channelFreq) {}


ImageCaptureInterface::CapErrorCode ATVCapture::initCapture()
{
    SYNC_PRINT(("ATVCapture::initCapture(): called\n"));

    if (centerFreq == -1)
    {
        SYNC_PRINT(("ATVCapture::initCapture(): Wrong center frequency value\n"));
        return ImageCaptureInterface::FAILURE;
    }
    // TODO: automatic chose of device

    try {
        SoapySDR::Kwargs args = SoapySDR::KwargsFromString("driver=hackrf");
        SDR = SoapySDR::Device::make(args);
    } catch (...) {
        SYNC_PRINT(("ATVCapture::initCapture(): Got an exeption from device creation\n"));
    }


    if (SDR == nullptr)
    {
        SYNC_PRINT(("ATVCapture::initCapture(): Unable to make a device\n"));
        return ImageCaptureInterface::FAILURE;
    }

    SDR->setSampleRate(SOAPY_SDR_RX, 0, sampleRate);
    SDR->setFrequency (SOAPY_SDR_RX, 0, centerFreq);
    rxStream = SDR->setupStream(SOAPY_SDR_RX, SOAPY_SDR_CF32);

    lumBackFrame  = new G12Buffer(cameraFormat.height, cameraFormat.width);
    lumFrontFrame = new G12Buffer(cameraFormat.height, cameraFormat.width);
    backFrame   = new RGB24Buffer(cameraFormat.height, cameraFormat.width);
    frontFrame  = new RGB24Buffer(cameraFormat.height, cameraFormat.width);

    input     = (fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * buffSize);
    output    = (fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * buffSize);
    freqLum   = (fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * buffSize);
    freqColor = (fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * buffSize);
    lum       = (fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * buffSize);
    color     = (fftwf_complex*) fftw_malloc(sizeof(fftwf_complex) * buffSize);

    forwardTransform       = fftwf_plan_dft_1d(buffSize, input,     output, FFTW_FORWARD,  FFTW_PATIENT);
    backwardLumTransform   = fftwf_plan_dft_1d(buffSize, freqLum,   lum,    FFTW_BACKWARD, FFTW_PATIENT);
    backwardColorTransform = fftwf_plan_dft_1d(buffSize, freqColor, color,  FFTW_BACKWARD, FFTW_PATIENT);

    SYNC_PRINT(("ATVCapture::initCapture(): exited\n"));
    return ImageCaptureInterface::SUCCESS;
}


ImageCaptureInterface::CapErrorCode ATVCapture::startCapture()
{
    SYNC_PRINT(("ATVCapture::startCapture(): called\n"));

    SDR->activateStream(rxStream, 0, 0, 0);
    mIsPaused = false;

    receiver = std::thread(&ATVCapture::receiving, this);
    filter   = std::thread(&ATVCapture::filtering, this);
    decoder  = std::thread(&ATVCapture::decoding,  this);

    SYNC_PRINT(("ATVCapture::startCapture(): exited\n"));
    return ImageCaptureInterface::SUCCESS;
}


void ATVCapture::receiving()
{
    for (u_char i = 3; !mIsPaused; (i == 3) ? (i = 0) : (i++))
    {
        bufferMutex[i].lock();

        void *buffs[] = {buff[i]};
        int flags;
        long long time_ns;
        SDR->readStream(rxStream, buffs, buffSize, flags, time_ns, 1e5);

        bufferMutex[i].unlock();
    }
}


void ATVCapture::filtering()
{
    float re, im;
    for (u_char i = 2; !mIsPaused; i = next_4(i))
    {
        bufferMutex[i].lock();

        // any filtering should be here

        for (u_short sampleNumber = 0; sampleNumber < buffSize; sampleNumber++)
        {
            input[sampleNumber][0] = buff[i][sampleNumber].real();
            input[sampleNumber][1] = buff[i][sampleNumber].imag();
        }

        fftwf_execute(forwardTransform);

        filterLum  (input, freqLum,   buffSize);
        filterColor(input, freqColor, buffSize);

        fftwf_execute(backwardLumTransform);
        fftwf_execute(backwardColorTransform);

        for (u_short sampleNumber = 0; sampleNumber < buffSize; sampleNumber++)
        {
            re = lum[sampleNumber][0]; im = lum[sampleNumber][1];
            buff[i][sampleNumber].real(fmin(100, sqrt(re * re + im * im)));
            buff[i][sampleNumber].imag(0 /* some info about color */);
        }

        bufferMutex[i].unlock();
    }
}


void ATVCapture::decoding()
{
    bufferMutex[0].lock();

    u_char signalState = IDLE;
    auto samplesCount = 0;
    u_short lineNumber = 0;
    u_short x;
    for (u_char i = 0; !mIsPaused; i = next_4(i))
    {
        bufferMutex[(i == 3) ? 0 : (i + 1)].lock();

        for (u_short sampleNumber = 0; sampleNumber < buffSize; sampleNumber++)
        {
            switch (signalState)
            {
                case IDLE:
                    if (buff[i][sampleNumber].real() > HORIZONTAL_SYNC_THRESHOLD &&
                        ((sampleNumber == buffSize - 1) ? buff[next_4(i)][0] : buff[i][sampleNumber + 1]).real() <
                        HORIZONTAL_SYNC_THRESHOLD)
                    {
                        signalState = HORIZONTAL_SYNC;
                        samplesCount = 0;
                    }
                    break;
                case HORIZONTAL_SYNC:
                    if (samplesCount > HORIZONTAL_SYNC_DURATION * sampleRate)
                    {
                        signalState = BACK_PORCH;
                        samplesCount = 0;
                    }
                    break;
                case BACK_PORCH:
                    if (samplesCount > BACK_PORCH_DURATION * sampleRate)
                    {
                        signalState = VIDEO;
                        samplesCount = 0;
                    }
                    break;
                case VIDEO:
                    x = int(cameraFormat.width * samplesCount / (VIDEO_DURATION * sampleRate));
                    lumBackFrame->element(lineNumber, x) = int(
                            (buff[i][sampleNumber].real() - BLACK_LEVEL) / (WHITE_LEVEL - BLACK_LEVEL) * 254);
                    if (samplesCount > VIDEO_DURATION * sampleRate)
                    {
                        signalState = FRONT_PORCH;
                        samplesCount = 0;
                        if (lineNumber == 525)
                            lineNumber = 2;
                        else
                            lineNumber += 2;
                    }
                    if (buff[i][sampleNumber].real() > HORIZONTAL_SYNC_THRESHOLD &&
                        ((sampleNumber == buffSize - 1) ? buff[next_4(i)][0] : buff[i][sampleNumber + 1]).real() <
                        HORIZONTAL_SYNC_THRESHOLD &&
                        samplesCount < 0.75 * VIDEO_DURATION * sampleRate)
                    {
                        signalState = VERTICAL_SYNC;
                        samplesCount = 0;
                    }
                    break;
                case FRONT_PORCH:
                    if (buff[i][sampleNumber].real() > HORIZONTAL_SYNC_THRESHOLD &&
                        ((sampleNumber == buffSize - 1) ? buff[next_4(i)][0] : buff[i][sampleNumber + 1]).real() <
                        HORIZONTAL_SYNC_THRESHOLD)
                    {
                        signalState = HORIZONTAL_SYNC;
                        samplesCount = 0;
                    }
                    if (samplesCount > FRONT_PORCH_DURATION * sampleRate)
                    {
                        signalState = HORIZONTAL_SYNC;
                        samplesCount = 0;
                    }
                    break;
                case VERTICAL_SYNC:
                    if (buff[i][sampleNumber].real() > VERTICAL_SYNC_LEVEL)
                    {
                        lineNumber = 0;
                        signalState = IDLE;
                        std::swap(lumBackFrame, lumFrontFrame);
                        std::swap(backFrame, frontFrame);
                        ImageCaptureInterface::FrameMetadata frameData{};
                        notifyAboutNewFrame(frameData);
                    }
                    break;
                default: SYNC_PRINT(("ATVCamera::decoding(): wrong signalState\n"));
            }
        }

        bufferMutex[i].unlock();
    }
    // bufferMutex[i].unlock();
}


ImageCaptureInterface::CapErrorCode ATVCapture::pauseCapture()
{
    SYNC_PRINT(("ATVCapture::pauseCapture(): called. Pause is %s\n", mIsPaused ? "ON" : "OFF"));

    if (mIsPaused)
    {
        mIsPaused = !mIsPaused;
        return startCapture();
    }
    else
    {
        mIsPaused = !mIsPaused;
        receiver.join();
        filter  .join();
        decoder .join();
        SDR->deactivateStream(rxStream, 0, 0);
        return ImageCaptureInterface::SUCCESS;
    }
}


bool ATVCapture::supportPause()
{   return true;   }


ImageCaptureInterface::FramePair ATVCapture::getFrame()
{
    FramePair result(nullptr, nullptr);
    result.setRgbBufferLeft(frontFrame);
    result.setBufferLeft(lumFrontFrame);
    return result;
}


ATVCapture::~ATVCapture()
{
    SYNC_PRINT(("ATVCapture::~ATVCapture(): called\n"));

    pauseCapture();

    fftwf_destroy_plan(forwardTransform);
    fftwf_destroy_plan(backwardLumTransform);
    fftwf_destroy_plan(backwardColorTransform);

    fftwf_free(input);
    fftwf_free(output);
    fftwf_free(freqLum);
    fftwf_free(freqColor);
    fftwf_free(lum);
    fftwf_free(color);

    SDR->closeStream(rxStream);
    SoapySDR::Device::unmake(SDR);

    SYNC_PRINT(("ATVCapture::~ATVCapture(): exited\n"));
}


double ATVCapture::getChannelFreq(std::string channel)
{
    unsigned short frequency;
    unsigned short frequencies[10][8] = {
            {5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725},
            {5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866},
            {5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945},
            {5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880},
            {5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917},
            {5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621},
            {5325, 5348, 5366, 5384, 5402, 5420, 5438, 5456},
            {5474, 5492, 5510, 5528, 5546, 5564, 5582, 5600},
            {5333, 5373, 5413, 5453, 5493, 5533, 5573, 5613},
            {5653, 5693, 5733, 5773, 5813, 5853, 5893, 5933}
    };

    if (channel[1] > '8' || channel[1] < '1')
    {
        SYNC_PRINT (("ATVCapture: Channel name is wrong\n"));
        return -1;
    }

    switch (channel[0])
    {
        case 'A': frequency = frequencies[0][channel[1] - 1 - '0']; break;
        case 'B': frequency = frequencies[1][channel[1] - 1 - '0']; break;
        case 'E': frequency = frequencies[2][channel[1] - 1 - '0']; break;
        case 'F': frequency = frequencies[3][channel[1] - 1 - '0']; break;
        case 'R': frequency = frequencies[4][channel[1] - 1 - '0']; break;
        case 'D': frequency = frequencies[5][channel[1] - 1 - '0']; break;
        case 'U': frequency = frequencies[6][channel[1] - 1 - '0']; break;
        case 'O': frequency = frequencies[7][channel[1] - 1 - '0']; break;
        case 'L': frequency = frequencies[8][channel[1] - 1 - '0']; break;
        case 'H': frequency = frequencies[9][channel[1] - 1 - '0']; break;
        default: SYNC_PRINT (("ATVCapture: Channel name is wrong\n")); return -1;
    }
    return 1e6 * frequency;
}

ImageCaptureInterface::CapErrorCode ATVCapture::queryCameraParameters(CameraParameters &parameter)
{
    parameter.mCameraControls[CameraParameters::GAIN].setActive      (true);
    parameter.mCameraControls[CameraParameters::GAIN].setMinimum     (0);
    parameter.mCameraControls[CameraParameters::GAIN].setMaximum     (100000);
    parameter.mCameraControls[CameraParameters::GAIN].setDefaultValue(50000);

    parameter.mCameraControls[CameraParameters::BRIGHTNESS].setActive      (true);
    parameter.mCameraControls[CameraParameters::BRIGHTNESS].setMinimum     (0);
    parameter.mCameraControls[CameraParameters::BRIGHTNESS].setMaximum     (100000);
    parameter.mCameraControls[CameraParameters::BRIGHTNESS].setDefaultValue(50000);

    parameter.mCameraControls[CameraParameters::CONTRAST].setActive      (true);
    parameter.mCameraControls[CameraParameters::CONTRAST].setMinimum     (0);
    parameter.mCameraControls[CameraParameters::CONTRAST].setMaximum     (100000);
    parameter.mCameraControls[CameraParameters::CONTRAST].setDefaultValue(50000);

    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode ATVCapture::setCaptureProperty(int id, int value)
{
    switch (id) {
        case CameraParameters::GAIN:
            gain = value;
            return  ImageCaptureInterface::SUCCESS;
        case CameraParameters::BRIGHTNESS:
            brightness = value;
            return  ImageCaptureInterface::SUCCESS;
        case CameraParameters::CONTRAST:
            contrast = value;
            return  ImageCaptureInterface::SUCCESS;
    default:
            break;

    }
    return  ImageCaptureInterface::FAILURE;
}

ImageCaptureInterface::CapErrorCode ATVCapture::getCaptureProperty(int id, int *value)
{
    switch (id) {
        case CameraParameters::GAIN:
            *value = gain;
            return  ImageCaptureInterface::SUCCESS;
        case CameraParameters::BRIGHTNESS:
            *value = brightness;
            return  ImageCaptureInterface::SUCCESS;
        case CameraParameters::CONTRAST:
            *value = contrast;
            return  ImageCaptureInterface::SUCCESS;
    default:
            break;

    }
    return  ImageCaptureInterface::FAILURE;

}
