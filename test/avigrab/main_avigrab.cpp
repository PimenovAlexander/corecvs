#include <stdio.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "global.h"
#include "rgb24Buffer.h"
#include "bmpLoader.h"

G12Buffer *oldFrame = NULL;

AbstractContiniousBuffer<double> *accumulator = NULL;
AbstractContiniousBuffer<double> *accumulator1 = NULL;


void processFrame(AVFrame* frame)
{
     //SYNC_PRINT(("We have a frame [%dx%d] of format %d\n", frame->height, frame->width, frame->format));
     if (frame->format != AV_PIX_FMT_YUV420P)
     {
         SYNC_PRINT(("Not supported format"));
         return;
     }

     RGB24Buffer *buffer = new RGB24Buffer(frame->height, frame->width);
     G12Buffer *buffer12 = new G12Buffer(frame->height, frame->width);
     for (int i = 0; i < frame->height; i++)
     {
         for (int j = 0; j < frame->width; j++)
         {
             uint8_t y = (frame->data[0])[i * frame->linesize[0] + j];
             uint8_t u = 128; (frame->data[1])[(i / 2) * frame->linesize[1] + (j / 2)];
             uint8_t v = 128; (frame->data[2])[(i / 2) * frame->linesize[2] + (j / 2)];
             buffer  ->element(i,j) = RGBColor::FromYUV(y,u,v);
             buffer12->element(i,j) = (int)y << 4;
         }
     }

     if (oldFrame == NULL)
     {
        accumulator  = new AbstractContiniousBuffer<double>(frame->height, frame->width);
        accumulator1 = new AbstractContiniousBuffer<double>(frame->height, frame->width);
     } else {
         for (int i = 0; i < frame->height; i++)
         {
             for (int j = 0; j < frame->width; j++)
             {
                 double diff = fabs(buffer12->element(i,j) - oldFrame->element(i,j));
                 accumulator ->element(i,j) += diff;
                 accumulator1->element(i,j) += (diff > (20 << 4)) ? 1 : 0;
             }
         }
         delete_safe(oldFrame);
     }

     oldFrame = buffer12;


     //SYNC_PRINT(("Saved frame\n"));
     //BMPLoader().save("test.bmp", buffer);
     //BMPLoader().save("test12.bmp", buffer12);

     delete_safe(buffer);
     //delete_safe(buffer12);
}

void dumpAccumulator(AbstractContiniousBuffer<double> *accumulator, std::string name)
{
    /*Saving acc*/
    double min = numeric_limits<double>::max();
    double max = 0;
    for (int i = 0; i < accumulator->h; i++)
    {
        for (int j = 0; j < accumulator->w; j++)
        {
            if (min > accumulator->element(i,j)) min = accumulator->element(i,j);
            if (max < accumulator->element(i,j)) max = accumulator->element(i,j);
        }
    }

    /* saving to G12 Buffer */
    G12Buffer *result = new G12Buffer(accumulator->getSize());
    for (int i = 0; i < accumulator->h; i++)
    {
        for (int j = 0; j < accumulator->w; j++)
        {
            result->element(i,j)  = (accumulator->element(i,j) - min) / (max-min) * G12Buffer::BUFFER_MAX_VALUE;
        }
    }
    BMPLoader().save(name, result);

    double lnmax = log(max + 1);
    for (int i = 0; i < accumulator->h; i++)
    {
        for (int j = 0; j < accumulator->w; j++)
        {
            result->element(i,j)  = log(accumulator->element(i,j) + 1) / lnmax * G12Buffer::BUFFER_MAX_VALUE;
        }
    }
    BMPLoader().save(std::string("ln_") + name , result);

}


int main (int argc, char **argv)
{
    printf("Raw avi read test. %d\n", argc);

    if (argc != 2)
    {
        printf("No input\n");
        return -1;
    }

    const char* filename = argv[1];

    SYNC_PRINT(("Starting the read of %s\n", filename));
    SYNC_PRINT(("Registering the codecs...\n"));
    av_register_all();

    AVFormatContext* format_context = NULL;

    int res;
    res = avformat_open_input(&format_context, filename, NULL, NULL);
    if (res < 0) {
        printf("Unable to open input file\n");
        return -1;
    }


    res = avformat_find_stream_info(format_context, NULL);
    if (res < 0) {
        printf("Unable to find stream info\n");
        return -1;
    }

    printf("File seem to have %d streams\n", format_context->nb_streams);

    // Dump information about file onto standard error
    av_dump_format(format_context, 0, argv[1], 0);


    // Find the first video stream
    int video_stream;
    for (video_stream = 0; video_stream < format_context->nb_streams; ++video_stream) {
        if (format_context->streams[video_stream]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            break;
        }
    }
    if (video_stream == format_context->nb_streams) {
        fprintf(stderr, "ffmpeg: Unable to find video stream\n");
        return -1;
    }
    printf("Video Stream found\n");
    AVCodecContext* codec_context = format_context->streams[video_stream]->codec;
    AVCodec* codec = avcodec_find_decoder(codec_context->codec_id);
    res = avcodec_open2(codec_context, codec, NULL);
    if (res < 0) {
        printf("Unable to open codec\n");
        return -1;
    }
    printf("Video codec found\n");

    AVFrame* frame = avcodec_alloc_frame();
    AVPacket packet;

    int count = 0;

    while (av_read_frame(format_context, &packet) >= 0) {
        if (packet.stream_index == video_stream) {
            // Video stream packet
            int frame_finished;
            avcodec_decode_video2(codec_context, frame, &frame_finished, &packet);

            if (frame_finished) {
               processFrame(frame);
               count++;
               if ((count % 100) == 0) {
                   SYNC_PRINT(("%d s...", count / 24));
               }
               if (count / 24 > 2000)
               {
                  SYNC_PRINT(("\nExiting\n"));
                   break;
               }
            }

            // Free the packet that was allocated by av_read_frame
            av_free_packet(&packet);
        }
    }

    // Free the YUV frame
    av_free(frame);

    // Close the codec
    avcodec_close(codec_context);

    // Close the video file
    avformat_close_input(&format_context);

    dumpAccumulator(accumulator , "result.bmp");
    dumpAccumulator(accumulator1, "result1.bmp");

    return 0;
}
