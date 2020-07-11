#ifndef AVENCODER_H
#define AVENCODER_H

#include "core/framesources/dummyVideoEncoderInterface.h"

extern "C" {
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class AVEncoder : public corecvs::DummyVideoEncoderInterface
{
public:
    AVEncoder();
    bool trace = false;


    bool open = false;

    virtual int startEncoding(const std::string &name, int h, int w, int codec_id = -1) override;
    virtual void addFrame(corecvs::RGB24Buffer *input) override;
    virtual void endEncoding() override;

    static void printCaps();


    static void rgb24BufferToAVFrameYUV420P(corecvs::RGB24Buffer *input, AVFrame *frame);
    static void rgb24BufferToAVFrameBGR8   (corecvs::RGB24Buffer *input, AVFrame *frame);

protected:
    int got_output = 0;
    int frame_number = 0;

    AVFormatContext *formatContext = NULL;
    AVOutputFormat  *outputFormat = NULL;

    AVFrame *frame = NULL;

    AVCodec *codec = NULL;
    AVCodecContext    *codecContext = NULL;
    //AVCodecParameters *codecParameters = NULL;

    AVStream *outStream = NULL;

    FILE *outFile = NULL;
};

#endif // AVENCODER_H
