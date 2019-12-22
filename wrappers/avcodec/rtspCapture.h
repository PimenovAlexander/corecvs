#ifndef RTSPCAPTURE_H
#define RTSPCAPTURE_H

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

class RTSPCapture : public virtual ImageCaptureInterface
{
public:
    RTSPCapture(const std::string &params);
    ~RTSPCapture();

    virtual ImageCaptureInterface::CapErrorCode initCapture() override;
    virtual ImageCaptureInterface::CapErrorCode startCapture() override;
    virtual CapErrorCode pauseCapture() override;
    virtual CapErrorCode nextFrame() override;
    virtual bool         supportPause() override;
    virtual FramePair    getFrame() override;


    std::string mName;
    AVFormatContext* mFormatContext = NULL;
    AVCodecParameters *mCodecParameters = NULL;
    AVCodecContext* mCodecContext = NULL;
    AVCodec* mCodec = NULL;

    bool mIsPaused;

    /*Codec related stuff*/
    AVFrame* mFrame;
    AVPacket mPacket;

    unsigned mVideoStream;

    int count;

    PreciseTimer mLastFrameTime;
};

#endif // RTSPCAPTURE_H
