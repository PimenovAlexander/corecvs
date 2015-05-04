#pragma once
/**
 * \file AviListBase.h
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

#ifdef WITH_SWSCALE
#include <libswscale/swscale.h>
#endif
}

#include "imageCaptureInterface.h"
#include "preciseTimer.h"

class AviListBase : public ImageCaptureInterface
{
    static bool avCodecInited;

public:
    AviListBase(QString const &params);
    ~AviListBase();

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

    int decodeFrame(AVFrame *frame, FramePair &framePair);
    virtual int getRawFrameFromAVI();

    std::string mName;
    AVFormatContext* mFormatContext;
    AVCodecContext* mCodecContext;
    AVCodec* mCodec;

    bool mIsPaused;

    /*Codec related stuff*/
    AVFrame* mFrame;
    AVPacket mPacket;
    unsigned mVideoStream;

    /* Some statistics related */
    uint64_t mVirualAVIStart;
    uint64_t usPerFrame;
    int count;
    PreciseTimer mLastFrameTime;


protected:

    string currentFile;
    int streamId;
    virtual string getNextFile();
    virtual ImageCaptureInterface::CapErrorCode prepareFile();
    void cleanup();

};
