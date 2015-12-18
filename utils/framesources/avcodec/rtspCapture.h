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

#include "imageCaptureInterface.h"
#include "preciseTimer.h"

class RTSPCapture : public ImageCaptureInterface
{
public:
    RTSPCapture(const QString &params);
    ~RTSPCapture();

    virtual ImageCaptureInterface::CapErrorCode initCapture();
    virtual ImageCaptureInterface::CapErrorCode startCapture();
    virtual CapErrorCode pauseCapture();
    virtual CapErrorCode nextFrame();
    virtual bool         supportPause();
    virtual FramePair    getFrame();


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

#endif // RTSPCAPTURE_H
