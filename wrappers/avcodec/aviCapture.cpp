/**
 * \brief Capture video stream from avi file using avcodec library
 */

#include "aviCapture.h"
#include "utils/preciseTimer.h"

extern "C" {
    #include <libavutil/pixdesc.h>
}


bool AviCapture::avCodecInited = false;

AviCapture::AviCapture(const std::string &params)
    : /*AbstractFileCapture(params),*/
       mName(params)
     , mFormatContext(NULL)
     , mCodecParameters(NULL)
     , mCodec(NULL)
     , mIsPaused(false)
     , mFrame(NULL)
     , count(1)
{
    SYNC_PRINT(("AviCapture::AviCapture(%s): called\n", params.c_str()));
    if (!avCodecInited)
    {
        //SYNC_PRINT(("Registering the codecs...\n"));
        //av_register_all();
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
        SYNC_PRINT(("AviCapture::initCapture(): failed to open file\n"));
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
    for (mVideoStream = 0; mVideoStream < (int)mFormatContext->nb_streams; mVideoStream++) {
        if (mFormatContext->streams[mVideoStream]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            break;
        }
    }

    if (mVideoStream == (int)mFormatContext->nb_streams) {
        SYNC_PRINT(("AviCapture::initCapture(): Unable to find video stream among %d streams\n", mFormatContext->nb_streams));
        return ImageCaptureInterface::FAILURE;
    }

    SYNC_PRINT(("AviCapture::initCapture(): Video Stream found\n"));
    mCodecParameters = mFormatContext->streams[mVideoStream]->codecpar;
    mCodec = avcodec_find_decoder(mCodecParameters->codec_id);    
    mCodecContext = avcodec_alloc_context3(mCodec);
    if (mCodecContext == NULL)
    {
        SYNC_PRINT(("AviCapture::initCapture(): Unable to allocate context\n"));
        return ImageCaptureInterface::FAILURE;
    }

    res = avcodec_parameters_to_context(mCodecContext, mCodecParameters);
    if (res < 0) {
        SYNC_PRINT(("AviCapture::initCapture(): Unable to set context parameters\n"));
        return ImageCaptureInterface::FAILURE;
    }

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

    //SYNC_PRINT(("AviCapture::getFrame(): called\n"));
    //mProtectFrame.lock();
        FramePair result(NULL, NULL);
        int res;
        while ( (res = av_read_frame(mFormatContext, &mPacket)) >= 0)
        {
            if (mPacket.stream_index == mVideoStream)
            {
                int frame_finished = false;
                int used = 0;

                //avcodec_decode_video2(mCodecContext, mFrame, &frame_finished, &mPacket);
                if (mCodecContext->codec_type == AVMEDIA_TYPE_VIDEO ||
                    mCodecContext->codec_type == AVMEDIA_TYPE_AUDIO)
                {
                    used = avcodec_send_packet(mCodecContext, &mPacket);
                    if (used < 0 && used != AVERROR(EAGAIN) && used != AVERROR_EOF) {
                    } else {
                         if (used >= 0) {
                            mPacket.size = 0;
                         }
                         used = avcodec_receive_frame(mCodecContext, mFrame);
                         if (used >= 0)
                             frame_finished = true;
                    }
                }

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
            if (mFrame->format == AV_PIX_FMT_YUV420P ||
                mFrame->format == AV_PIX_FMT_YUVJ420P)
            {
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
             } else if (mFrame->format == AV_PIX_FMT_YUV422P ) {
                SYNC_PRINT(("AviCapture::getFrame(): format AV_PIX_FMT_YUV422P \n"));

                return result;
             } else {
                SYNC_PRINT(("AviCapture::getFrame(): Not supported format %d <%s>\n"
                            , mFrame->format, av_get_pix_fmt_name((AVPixelFormat)mFrame->format)
                            ));
#if 0
                SYNC_PRINT(("We support so far:\n"));
                SYNC_PRINT((" - %d %s\n", ));
#endif

                return result;
             }
        } else {
            SYNC_PRINT(("AviCapture::getFrame(): av_read_frame failed with %d [0x%x %c %c %c]\n", res,
                        (uint8_t)(((uint32_t)res & 0xFF000000) >> 24),
                        (uint8_t)(((uint32_t)res & 0x00FF0000) >> 16),
                        (uint8_t)(((uint32_t)res & 0x0000FF00) >> 8 ),
                        (uint8_t)(((uint32_t)res & 0x000000FF)      )));


            char errorText[100];
            av_make_error_string (errorText, CORE_COUNT_OF(errorText), res);
            SYNC_PRINT(("Error string: <%s>\n", errorText));

            return result;
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
        ImageCaptureInterface::FrameMetadata frameData;
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
    ImageCaptureInterface::FrameMetadata frameData;

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
    ImageCaptureInterface::FrameMetadata frameData;
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

