/**
 * \brief Capture video stream from avi file using avcodec library
 */
#include <QtCore/QRegExp>
#include <QtCore/QString>

#include <QtCore/QDateTime>

#include "preciseTimer.h"

#include "aviListBase.h"


bool AviListBase::avCodecInited = false;

AviListBase::AviListBase(QString const &params)
    : /*AbstractFileCapture(params),*/
       mName(params.toStdString())
     , mFormatContext(NULL)
     , mCodecContext(NULL)
     , mCodec(NULL)
     , mIsPaused(false)
     , mFrame(NULL)
     , mVirualAVIStart(1)
     , usPerFrame(1000000UL / 25)
     , count(1)
     , streamId(0)
{
    //SYNC_PRINT(("AviListBase::AviListBase(%s): called\n", params.toLatin1().constData()));

    printf("AVI is assumed generic AVI:%s\n", params.toLatin1().constData());
    mVirualAVIStart = PreciseTimer::currentTime().usec();

    if (!avCodecInited)
    {
        //SYNC_PRINT(("Registering the codecs...\n"));
        av_register_all();
        avCodecInited = true;
    }

    //SYNC_PRINT(("AviListBase::AviListBase(): exited\n"));
}

ImageCaptureInterface::CapErrorCode AviListBase::initCapture()
{
    ImageCaptureInterface::CapErrorCode res = prepareFile();
    return res;
}

int AviListBase::decodeFrame(AVFrame* frame, FramePair &framePair)
{
    if (frame->format != PIX_FMT_YUV420P && frame->format != PIX_FMT_YUVJ420P)
    {
        SYNC_PRINT(("AviListBase::getFrame(): Not supported format %d\n", mFrame->format));
        return 1;
    }

    framePair.rgbBufferLeft = new RGB24Buffer(frame->height, frame->width);
    framePair.bufferLeft    = new G12Buffer  (frame->height, frame->width);

#ifndef WITH_SWSCALE
    for (int i = 0; i < frame->height; i++)
    {
        uint8_t *yPtr = &(frame->data[0][ i      * frame->linesize[0]]);
        uint8_t *uPtr = &(frame->data[1][(i / 2) * frame->linesize[1]]);
        uint8_t *vPtr = &(frame->data[2][(i / 2) * frame->linesize[2]]);


        for (int j = 0; j < frame->width; j+=2)
        {
            uint8_t y1 = *yPtr;
            uint8_t y2 = *(yPtr + 1);
            uint8_t u = *uPtr;
            uint8_t v = *vPtr;

            framePair.rgbBufferLeft->element(i,j    ) = RGBColor::FromYUV(y1,u,v);
            framePair.rgbBufferLeft->element(i,j + 1) = RGBColor::FromYUV(y2,u,v);

            framePair.bufferLeft   ->element(i,j    ) = (int)y1 << 4;
            framePair.bufferLeft   ->element(i,j + 1) = (int)y2 << 4;

            yPtr += 2;
            uPtr ++;
            vPtr ++;
        }
    }
#else
    SwsContext * ctx = sws_getContext(frame->width, frame->height,
                                      PIX_FMT_YUV420P,
                                      framePair.rgbBufferLeft->w, framePair.rgbBufferLeft->h,
                                      PIX_FMT_RGB32, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    if (ctx == NULL) {
        SYNC_PRINT(("AviListBase::getFrame(): ctx context is null\n"));
    }

    uint8_t * outData[1] = { (uint8_t *)framePair.rgbBufferLeft->data }; // RGB24 have one plane
    int outLinesize[1] = { framePair.rgbBufferLeft->stride * sizeof(RGB24Buffer::InternalElementType) }; // RGB stride
    sws_scale(ctx, frame->data, frame->linesize, 0, frame->height, outData, outLinesize);
    sws_freeContext(ctx);
    ctx = NULL;

#endif

    framePair.rgbBufferRight = new RGB24Buffer(framePair.rgbBufferLeft);
    framePair.bufferRight    = new G12Buffer(framePair.bufferLeft);
    return 0;
}


int AviListBase::getRawFrameFromAVI()
{
    int res;
    while ( (res = av_read_frame(mFormatContext, &mPacket)) >= 0)
    {
        if (mPacket.stream_index == mVideoStream)
        {
            int frame_finished;
            avcodec_decode_video2(mCodecContext, mFrame, &frame_finished, &mPacket);
            av_free_packet(&mPacket);

            if (frame_finished) {
//                    //SYNC_PRINT(("AviListBase::getFrame(): Frame ready\n"));
                break;
            }
        } else {
            av_free_packet(&mPacket);
        }
    }
    return res;
}

string AviListBase::getNextFile()
{
    return "";
}

ImageCaptureInterface::FramePair AviListBase::getFrame()
{
    CaptureStatistics  stats;


//    //SYNC_PRINT(("AviListBase::getFrame(): called\n"));

    FramePair result(NULL, NULL);
    int res = 0;
    PreciseTimer start = PreciseTimer::currentTime();
    while (true) {
        res = getRawFrameFromAVI();
        if (res >= 0) {
            break; /* New frame is fetched*/
        }
        /* No new frame, so we try to load next file */
        mName = getNextFile();
        streamId++;
        count = 0;
        if (!mName.empty()) {
            prepareFile();
        } else {
            break; /* no luck at all */
        }
    }
    stats.values[CaptureStatistics::DECODING_TIME] = start.usecsToNow();

    start = PreciseTimer::currentTime();
    if (res >= 0)
    {
        if (decodeFrame(mFrame, result) != 0) {
            return result;
        }
    } else {
        /* Ok we are out of frames and files... */
    }

    result.leftTimeStamp  = mVirualAVIStart + count * usPerFrame;
    result.rightTimeStamp = mVirualAVIStart + count * usPerFrame;
    result.streamId = streamId;


    stats.values[CaptureStatistics::CONVERTING_TIME] = start.usecsToNow();

    if (mLastFrameTime.usecsTo(PreciseTimer()) != 0)
    {
        stats.values[CaptureStatistics::INTERFRAME_DELAY] = mLastFrameTime.usecsToNow();
    }
    mLastFrameTime = PreciseTimer::currentTime();
    stats.values[CaptureStatistics::DATA_SIZE] = 0;
    emit newStatisticsReady(stats);

    if (!mIsPaused)
    {
        // SYNC_PRINT(("AviListBase::getFrame(): New notification sending\n"));
        count++;
        frame_data_t frameData;
        frameData.timestamp = mVirualAVIStart + count * usPerFrame;
        frameData.streamId = streamId;
        notifyAboutNewFrame(frameData);
    } else {
        //SYNC_PRINT(("AviListBase::getFrame(): Paused\n"));
    }

    return result;
}

ImageCaptureInterface::CapErrorCode AviListBase::startCapture()
{
//  return ImageCaptureInterface::CapSuccess1Cam;
    //SYNC_PRINT(("AviListBase::startCapture(): called\n"));
    frame_data_t frameData;

    //mIsPaused = false;

    count++;
    frameData.timestamp = mVirualAVIStart + count * usPerFrame;
    frameData.streamId = streamId;
    //SYNC_PRINT(("AviListBase::startCapture(): sending notification\n"));
    notifyAboutNewFrame(frameData);


    //SYNC_PRINT(("AviListBase::startCapture(): exited\n"));
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode AviListBase::pauseCapture()
{
    mIsPaused = !mIsPaused;
    //SYNC_PRINT(("AviListBase::pauseCapture(): called. Pause is %s\n", mIsPaused ? "ON" : "OFF"));
    if (!mIsPaused)
    {
        nextFrame();
    }

    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode AviListBase::nextFrame()
{
    count++;
    frame_data_t frameData;
    frameData.timestamp = mVirualAVIStart + count * usPerFrame;
    frameData.streamId = streamId;
    //SYNC_PRINT(("AviListBase::nextFrame(): sending notification\n"));
    notifyAboutNewFrame(frameData);

    return ImageCaptureInterface::SUCCESS;
}

bool AviListBase::supportPause()
{
    return true;
}

ImageCaptureInterface::CapErrorCode AviListBase::prepareFile()
{
    //SYNC_PRINT(("AviListBase::initCapture(): called\n"));
    cleanup();

    int res;
    res = avformat_open_input(&mFormatContext, mName.c_str(), NULL, NULL);
    if (res < 0) {
        //SYNC_PRINT(("AviListBase::initCapture(): failed to open file <%s>", mName.c_str()));
        return ImageCaptureInterface::FAILURE;
    }

    res = avformat_find_stream_info(mFormatContext, NULL);
    if (res < 0) {
        //SYNC_PRINT(("AviListBase::initCapture(): Unable to find stream info\n"));
        return ImageCaptureInterface::FAILURE;
    }

    //SYNC_PRINT(("File seem to have %d streams\n", mFormatContext->nb_streams));

    // Dump information about file onto standard error
    //av_dump_format(mFormatContext, 0, mName.c_str(), 0);

    // Find the first video stream
    for (mVideoStream = 0; mVideoStream < mFormatContext->nb_streams; mVideoStream++) {
        if (mFormatContext->streams[mVideoStream]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            break;
        }
    }

    if (mVideoStream == mFormatContext->nb_streams) {
        //SYNC_PRINT(("AviListBase::initCapture(): Unable to find video stream among %d streams\n", mFormatContext->nb_streams));
        return ImageCaptureInterface::FAILURE;
    }

    //SYNC_PRINT(("AviListBase::initCapture(): Video Stream found\n"));
    AVStream *stream = mFormatContext->streams[mVideoStream];
    mCodecContext = stream->codec;
    mCodec = avcodec_find_decoder(mCodecContext->codec_id);
    res = avcodec_open2(mCodecContext, mCodec, NULL);
    if (res < 0) {
        //SYNC_PRINT(("AviListBase::initCapture(): Unable to open codec\n"));
        return ImageCaptureInterface::FAILURE;
    }
    //SYNC_PRINT(("AviListBase::initCapture(): Video codec found\n"));

    mFrame = avcodec_alloc_frame();

    /*SYNC_PRINT(("AviListBase::initCapture(): Stream FPS is r=%d/%d avg=%d/%d\n",
                stream->r_frame_rate.num,
                stream->r_frame_rate.den,

                stream->avg_frame_rate.num,
                stream->avg_frame_rate.den
    ));*/

    do {
        if ( stream->avg_frame_rate.den != 0 &&  stream->avg_frame_rate.num != 0) {
            usPerFrame = 1000000UL * stream->avg_frame_rate.den / stream->avg_frame_rate.num;
            break;
        }

        if ( stream->r_frame_rate.den != 0 && stream->r_frame_rate.num != 0) {
            usPerFrame = 1000000UL * stream->r_frame_rate.den / stream->r_frame_rate.num;
            break;
        }
        usPerFrame = 1000000UL / 25;
    } while (false);

    //SYNC_PRINT(("AviListBase::initCapture(): Stream us per frame is %" PRIu64 "\n", usPerFrame));

    //SYNC_PRINT(("AviListBase::initCapture(): exited\n"));
    return ImageCaptureInterface::SUCCESS_1CAM;
}

void AviListBase::cleanup()
{
    //SYNC_PRINT(("AviListBase::cleanup(): called\n"));
    if (mFrame != NULL) {
        av_free(mFrame);
        mFrame = NULL;
    }
    if (mCodecContext != NULL) {
        avcodec_close(mCodecContext);
        mCodecContext = NULL;
    }
    if (mFormatContext != NULL) {
        avformat_close_input(&mFormatContext);
        mFormatContext = NULL;
    }
    //SYNC_PRINT(("AviListBase::cleanup(): exited\n"));
}

AviListBase::~AviListBase()
{
    //SYNC_PRINT(("AviListBase::~AviListBase(): called\n"));
    cleanup();
    //SYNC_PRINT(("AviListBase::~AviListBase(): exited\n"));
}

