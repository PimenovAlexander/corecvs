#pragma once
/**
 * \file aviCapture.h
 * \brief Add Comment Here
 *
 * \date Jan 9, 2015
 * \author alexander
 */


//#define __STDC_CONSTANT_MACROS
//#include </usr/include/stdint.h>

extern "C" {

# if __WORDSIZE == 64
#  define INT64_C(c)	c ## L
# else
#  define INT64_C(c)	c ## LL
# endif
# if __WORDSIZE == 64
#  define UINT64_C(c)	c ## UL
# else
#  define UINT64_C(c)	c ## ULL
# endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "core/framesources/imageCaptureInterface.h"
#include "core/utils/preciseTimer.h"

class AviCapture : public virtual ImageCaptureInterface
{
    static bool avCodecInited;

public:
    AviCapture(std::string const &params);
    ~AviCapture();

    virtual ImageCaptureInterface::CapErrorCode initCapture()  override;
    virtual ImageCaptureInterface::CapErrorCode startCapture() override;
    virtual CapErrorCode pauseCapture() override;
    virtual CapErrorCode nextFrame() override;

    /**
     * Check if particular image capturer support pausing.
     * This function can be called before starting or even initing the capture
     **/
    virtual bool         supportPause() override;

    virtual FramePair getFrame() override;

    std::string mName;
    AVFormatContext* mFormatContext;
    AVCodecContext* mCodecContext;
    AVCodec* mCodec;

    bool mIsPaused;

    /*Codec related stuff*/
    AVFrame* mFrame;
    AVPacket mPacket;

    unsigned mVideoStream;

    int count;

    PreciseTimer mLastFrameTime;
};


class AVICaptureProducer : public ImageCaptureInterfaceProducer
{
public:
    AVICaptureProducer()
    {}

    virtual std::string getPrefix() override
    {
        return "avcodec:";
    }

    virtual ImageCaptureInterface *produce(std::string &name, bool /*isRGB*/) override
    {
        return new AviCapture(name);
    }
};

