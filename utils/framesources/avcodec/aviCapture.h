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

#include "imageCaptureInterface.h"
#include "preciseTimer.h"

class AviCapture : public ImageCaptureInterface
{
    static bool avCodecInited;

public:
    AviCapture(QString const &params);
    ~AviCapture();

    virtual ImageCaptureInterface::CapErrorCode initCapture();
    virtual ImageCaptureInterface::CapErrorCode startCapture();
    virtual CapErrorCode pauseCapture();
    virtual CapErrorCode nextFrame();

    /**
     * Check if particular image capturer support pausing.
     * This function can be called before starting or even initing the capture
     **/
    virtual bool         supportPause();

    virtual FramePair getFrame();

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
