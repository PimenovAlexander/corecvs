/**
 * \brief Capture video stream from avi file using avcodec library
 */
#include <QtCore/QRegExp>
#include <QtCore/QString>

#include "core/utils/preciseTimer.h"

#include "aviCapture.h"


bool AviCapture::avCodecInited = false;

AviCapture::AviCapture(const std::string &params)
    : /*AbstractFileCapture(params),*/
       mName(params)
     , mFormatContext(NULL)
     , mCodecContext(NULL)
     , mCodec(NULL)
     , mIsPaused(false)
     , mFrame(NULL)
     , count(1)
{
    SYNC_PRINT(("AviCapture::AviCapture(%s): called\n", params.c_str()));
    if (!avCodecInited)
    {
        SYNC_PRINT(("Registering the codecs...\n"));
        av_register_all();
        avCodecInited = true;
    }

    SYNC_PRINT(("AviCapture::AviCapture(): exited\n"));
}

ImageCaptureInterface::CapErrorCode AviCapture::initCapture()
{
    SYNC_PRINT(("AviCapture::initCapture(): called\n"));

    int res;
    res = avformat_open_input(&mFormatContext, mName.c_str(), NULL, NULL);
    if (res < 0) {
        SYNC_PRINT(("AviCapture::initCapture(): failed to open file"));
        return ImageCaptureInterface::FAILURE;
    }

    res = avformat_find_stream_info(mFormatContext, NULL);
    if (res < 0) {
        SYNC_PRINT(("AviCapture::initCapture(): Unable to find stream info\n"));
        return ImageCaptureInterface::FAILURE;
    }

    SYNC_PRINT(("File seem to have %d streams\n", mFormatContext->nb_streams));

    // Dump information about file onto standard error
    av_dump_format(mFormatContext, 0, mName.c_str(), 0);

    // Find the first video stream
    for (mVideoStream = 0; mVideoStream < mFormatContext->nb_streams; mVideoStream++) {
        if (mFormatContext->streams[mVideoStream]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            break;
        }
    }

    if (mVideoStream == mFormatContext->nb_streams) {
        SYNC_PRINT(("AviCapture::initCapture(): Unable to find video stream among %d streams\n", mFormatContext->nb_streams));
        return ImageCaptureInterface::FAILURE;
    }

    SYNC_PRINT(("AviCapture::initCapture(): Video Stream found\n"));
    mCodecContext = mFormatContext->streams[mVideoStream]->codec;
    mCodec = avcodec_find_decoder(mCodecContext->codec_id);
    res = avcodec_open2(mCodecContext, mCodec, NULL);
    if (res < 0) {
        SYNC_PRINT(("AviCapture::initCapture(): Unable to open codec\n"));
        return ImageCaptureInterface::FAILURE;
    }
    SYNC_PRINT(("AviCapture::initCapture(): Video codec found\n"));

    mFrame = av_frame_alloc();

    SYNC_PRINT(("AviCapture::initCapture(): exited\n"));
    return ImageCaptureInterface::SUCCESS_1CAM;
}

ImageCaptureInterface::FramePair AviCapture::getFrame()
{
    CaptureStatistics  stats;
    PreciseTimer start = PreciseTimer::currentTime();

//    SYNC_PRINT(("AviCapture::getFrame(): called\n"));
    //mProtectFrame.lock();
        FramePair result(NULL, NULL);
        int res;
        while ( (res = av_read_frame(mFormatContext, &mPacket)) >= 0)
        {
            if (mPacket.stream_index == mVideoStream)
            {
                int frame_finished;
                avcodec_decode_video2(mCodecContext, mFrame, &frame_finished, &mPacket);
                av_packet_unref(&mPacket);
                if (frame_finished) {
//                    SYNC_PRINT(("AviCapture::getFrame(): Frame ready\n"));
                    break;
                }
            } else {
                av_packet_unref(&mPacket);
            }
        }

        if (res >= 0)
        {
            if (mFrame->format != AV_PIX_FMT_YUV420P && mFrame->format != AV_PIX_FMT_YUVJ420P)
            {
                SYNC_PRINT(("AviCapture::getFrame(): Not supported format %d\n", mFrame->format));
                return result;
            }

            result.setRgbBufferLeft(new RGB24Buffer(mFrame->height, mFrame->width));
            result.setBufferLeft   (new G12Buffer  (mFrame->height, mFrame->width));
            for (int i = 0; i < mFrame->height; i++)
            {
                for (int j = 0; j < mFrame->width; j++)
                {
                    uint8_t y = (mFrame->data[0])[i * mFrame->linesize[0] + j];

                    uint8_t u = (mFrame->data[1])[(i / 2) * mFrame->linesize[1] + (j / 2)];
                    uint8_t v = (mFrame->data[2])[(i / 2) * mFrame->linesize[2] + (j / 2)];

                    result.rgbBufferLeft()->element(i,j) = RGBColor::FromYUV(y,u,v);
                    result.bufferLeft()   ->element(i,j) = (int)y << 4;
                }
            }

            result.setRgbBufferRight (new RGB24Buffer(result.rgbBufferLeft()));
            result.setBufferRight    (new G12Buffer(result.bufferLeft()));
        }


        result.setTimeStampLeft (count * 10);
        result.setTimeStampRight(count * 10);

    //mProtectFrame.unlock();
    stats.values[CaptureStatistics::DECODING_TIME] = start.usecsToNow();

    if (mLastFrameTime.usecsTo(PreciseTimer()) != 0)
    {
        stats.values[CaptureStatistics::INTERFRAME_DELAY] = mLastFrameTime.usecsToNow();
    }
    mLastFrameTime = PreciseTimer::currentTime();
    stats.values[CaptureStatistics::DATA_SIZE] = 0;
    if (imageInterfaceReceiver != NULL)
    {
        imageInterfaceReceiver->newStatisticsReadyCallback(stats);
    }

    if (!mIsPaused)
    {
        //SYNC_PRINT(("AviCapture::getFrame(): New notification sending\n"));
        count++;
        frame_data_t frameData;
        frameData.timestamp = (count * 10);
        notifyAboutNewFrame(frameData);
    } else {
        SYNC_PRINT(("AviCapture::getFrame(): Paused\n"));
    }

    //count++;

    return result;
}

ImageCaptureInterface::CapErrorCode AviCapture::startCapture()
{
//  return ImageCaptureInterface::CapSuccess1Cam;
    SYNC_PRINT(("AviCapture::startCapture(): called\n"));
    frame_data_t frameData;

    //mIsPaused = false;

    count++;
    frameData.timestamp = (count * 10);
    SYNC_PRINT(("AviCapture::startCapture(): sending notification\n"));
    notifyAboutNewFrame(frameData);


    SYNC_PRINT(("AviCapture::startCapture(): exited\n"));
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode AviCapture::pauseCapture()
{
    mIsPaused = !mIsPaused;
    SYNC_PRINT(("AviCapture::pauseCapture(): called. Pause is %s\n", mIsPaused ? "ON" : "OFF"));
    if (!mIsPaused)
    {
        nextFrame();
    }

    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode AviCapture::nextFrame()
{
    count++;
    frame_data_t frameData;
    frameData.timestamp = (count * 10);
    SYNC_PRINT(("AviCapture::nextFrame(): sending notification\n"));
    notifyAboutNewFrame(frameData);

    return ImageCaptureInterface::SUCCESS;
}

bool AviCapture::supportPause()
{
    return true;
}

AviCapture::~AviCapture()
{
    SYNC_PRINT(("AviCapture::~AviCapture(): called\n"));

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

    SYNC_PRINT(("AviCapture::~AviCapture(): exited\n"));
}

