#pragma once
/**
 * \file aviCapture.h
 * \brief Add Comment Here
 *
 * \date Jan 9, 2015
 * \author alexander
 */
#include <cstddef>  // GCC 4.6 needs this to compile

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "imageCaptureInterface.h"

class AviCapture : public ImageCaptureInterface
{
    static bool avCodecInited;

public:
    AviCapture(QString const &params);
    ~AviCapture();

    virtual ImageCaptureInterface::CapErrorCode initCapture();
    virtual ImageCaptureInterface::CapErrorCode startCapture();
    virtual FramePair getFrame();

    std::string mName;
    AVFormatContext* mFormatContext;
    AVCodecContext* mCodecContext;
    AVCodec* mCodec;

    /*Codec related stuff*/
    AVFrame* mFrame;
    AVPacket mPacket;

    unsigned mVideoStream;

    int count;


};
