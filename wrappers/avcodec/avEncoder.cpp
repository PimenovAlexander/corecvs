#include "avEncoder.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
}

AVEncoder::AVEncoder()
{

}

int AVEncoder::startEncoding(const std::string &name, int h, int w, int codec_id )
{
    printf("AVEncoder::startEncoding(%s):called\n", name.c_str());

    int ret;

    if (trace) SYNC_PRINT(("Allocate the output media context...\n"));
    avformat_alloc_output_context2(&formatContext, NULL, NULL, name.c_str());
    if (formatContext == NULL) {
        SYNC_PRINT(("Could not deduce output format from file extension: using MPEG.\n"));
        avformat_alloc_output_context2(&formatContext, NULL, "mpeg", name.c_str());
    }

    if (formatContext == NULL) {
        SYNC_PRINT(("formatContext is NULL.\n"));
        return 1;
    }
    outputFormat = formatContext->oformat;


    if (codec_id == -1) {
        if (std::string(outputFormat->name) == "gif")
        {
            codec_id = AV_CODEC_ID_GIF;
        } else {
            codec_id = AV_CODEC_ID_MPEG4;
        }
    }

    if (trace) SYNC_PRINT(("Find video encoder...\n"));
     codec = avcodec_find_encoder((AVCodecID)codec_id);
     if (codec == NULL) {
         fprintf(stderr, "Codec not found\n");
         return 1;
     }

     if (trace) SYNC_PRINT(("Creating video stream...\n"));
     outStream = avformat_new_stream(formatContext, codec);
     if (outStream == NULL) {         
         fprintf(stderr, "Could not allocate stream\n");
         return 1;
     }
     outStream->id = formatContext->nb_streams - 1;
     if (trace) SYNC_PRINT(("Created stream pointer %p vs %p...\n", outStream, formatContext->streams[outStream->id]));



     SYNC_PRINT(("Stream id %d\n", outStream->id));

     if (trace) SYNC_PRINT(("Creating codec context...\n"));
     codecContext = avcodec_alloc_context3(codec);
     if (codecContext == NULL) {
         fprintf(stderr, "Could not allocate codec context\n");
         return 1;
     }

     if (outStream->codecpar == NULL) {
         fprintf(stderr, "codecParameters is NULL\n");
         return 1;
     }

     if (trace) SYNC_PRINT(("Preparing sample parameters...\n"));
     outStream->codecpar->bit_rate = 12400000;
     outStream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
     /* resolution must be a multiple of two */
     outStream->codecpar->width = w;
     outStream->codecpar->height = h;

     /* frames per second */
     outStream->time_base = AVRational({1,25});

     avcodec_parameters_to_context(codecContext, outStream->codecpar);

     codecContext->bit_rate = outStream->codecpar->bit_rate;
     codecContext->codec_id = codec->id;
     codecContext->height = outStream->codecpar->height;
     codecContext->width  = outStream->codecpar->width;
     codecContext->time_base = AVRational({1,25});
     codecContext->gop_size = 10; /* emit one intra frame every ten frames */
     codecContext->max_b_frames=1;
     if (codec->id == AV_CODEC_ID_GIF) {
        codecContext->pix_fmt = AV_PIX_FMT_BGR8;
     } else{
        codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
     }

     if(codec->id == AV_CODEC_ID_H264) {
         av_opt_set(codecContext, "preset", "slow", 0);
     }
     if (outputFormat->flags & AVFMT_GLOBALHEADER) {
         codecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
     }

     avcodec_parameters_from_context(outStream->codecpar, codecContext);


     if (trace) SYNC_PRINT(("Preparing muxer...\n"));
     if (!(outputFormat->flags & AVFMT_NOFILE)) {
         ret = avio_open(&formatContext->pb, name.c_str(), AVIO_FLAG_WRITE);
         if (ret < 0) {
             fprintf(stderr, "Could not open '%s': %d\n", name.c_str(), ret);
             return 1;
         }
     }


     /* open codec */
     if (trace) SYNC_PRINT(("Opening codec...\n"));
     if (avcodec_open2(codecContext, codec, NULL) < 0) {
         fprintf(stderr, "Could not open codec\n");
         return 1;
     }

     if (trace) SYNC_PRINT(("Allocating frame...\n"));
     frame = av_frame_alloc();
     if (!frame) {
         fprintf(stderr, "Could not allocate video frame\n");
         return 1;
     }
     frame->format = codecContext->pix_fmt;
     frame->width  = codecContext->width;
     frame->height = codecContext->height;

     /* the image can be allocated by any means and av_image_alloc() is
      * just the most convenient way if av_malloc() is to be used */
     ret = av_image_alloc(frame->data,
                          frame->linesize,
                          codecContext->width,
                          codecContext->height,
                          codecContext->pix_fmt, 32);
     if (ret < 0) {
         fprintf(stderr, "Could not allocate raw picture buffer\n");
         exit(1);
     }


     if (trace) SYNC_PRINT(("Current format...\n"));
     av_dump_format(formatContext, 0, name.c_str(), 1);

     /**
      * Some tracing....
      **/
     {
         AVStream *stream = formatContext->streams[0];

         if (trace) SYNC_PRINT(("Writing header...\n"));
         if (trace) SYNC_PRINT(("Format dimentions:\n"));
         if (trace) SYNC_PRINT(("    codecpar [%d %d] <%s>\n",
                stream->codecpar->width,
                stream->codecpar->height,
                av_get_media_type_string(stream->codecpar->codec_type) ));

         if (trace) SYNC_PRINT(("    codec    [%d %d]\n",
                stream->codec->width,
                stream->codec->height));
     }
     ret = avformat_write_header(formatContext, NULL);
     if (ret < 0) {
         fprintf(stderr, "Writing format problem\n");
         return ret;
     }

     open = true;

     if (trace) SYNC_PRINT(("startEncoding(): Function finished...\n"));
     return 0;
}



void AVEncoder::addFrame(corecvs::RGB24Buffer *input)
{
    if (trace) SYNC_PRINT(("AVEncoder::addFrame([%d %d]): called\n", input->w, input->h));

    int ret;

    if (input == NULL)
        return;

    AVPacket packet;
    av_init_packet(&packet);
    packet.data = NULL;    // packet data will be allocated by the encoder
    packet.size = 0;

    /* prepare a dummy image */
    if (codec->id == AV_CODEC_ID_GIF) {
        rgb24BufferToAVFrameBGR8   (input, frame);
    } else {
        rgb24BufferToAVFrameYUV420P(input, frame);
    }

     frame_number++;

     frame->pts = frame_number;

     /* encode the image */
     if (trace) SYNC_PRINT(("Sending frame to codec\n"));
     ret = avcodec_send_frame(codecContext, frame);
     if (ret < 0) {
         fprintf(stderr, "Error encoding frame\n");
         exit(1);
     }


     printf("Write frame %3d (size=%5d)\n", frame_number, packet.size);
     if (trace) SYNC_PRINT(("Reciveing packet from codec\n"));
     ret = avcodec_receive_packet(codecContext, &packet);
     if (ret == 0) {
         fprintf(stderr, "Got packet from codec\n");
         if ((ret = av_interleaved_write_frame(formatContext, &packet)) < 0) {
             fprintf(stderr, "Failed to write packet\n");
             return;
         }
     }


}

void AVEncoder::endEncoding()
{

    if (trace) SYNC_PRINT(("AVEncoder::endEncoding():called\n"));
    av_write_trailer(formatContext);

    //fclose(outFile);
    outFile = NULL;

    avcodec_close(codecContext);
    av_free(codecContext);
    av_freep(&frame->data[0]);
    av_frame_free(&frame);
    printf("\n");
    open = false;
}

#if FF_API_NEXT
void AVEncoder::printCaps()
{
    //av_register_all();
    //avcodec_register_all();

    SYNC_PRINT(("AVEncoder::printCaps():format list\n"));
    void *opaque = NULL;

    const AVOutputFormat * oformat = av_muxer_iterate(&opaque);
    while(oformat != NULL)
    {
        SYNC_PRINT(("%s %s\n", oformat->name, oformat->long_name));
        oformat = av_muxer_iterate(&opaque);
    }

    SYNC_PRINT(("=============\n"));

    SYNC_PRINT(("AVEncoder::printCaps():codec list\n"));

    opaque = NULL;
    const AVCodec *codec = NULL;
    while (true)
    {
        codec = av_codec_iterate(&opaque);
        if (codec == NULL)
        {
            break;
        }
        SYNC_PRINT(("0x%4.4X %s %s\n", codec->id, codec->name, codec->long_name));
    }
    SYNC_PRINT(("=============\n"));

}

#else
void AVEncoder::printCaps()
{
    av_register_all();
    avcodec_register_all();


    SYNC_PRINT(("AVEncoder::printCaps():format list\n"));
    AVOutputFormat * oformat = av_oformat_next(NULL);
    while(oformat != NULL)
    {
        SYNC_PRINT(("%s %s\n", oformat->name, oformat->long_name));
        oformat = av_oformat_next(oformat);
    }

    SYNC_PRINT(("=============\n"));

    SYNC_PRINT(("AVEncoder::printCaps():codec list\n"));

    AVCodec *codec = NULL;
    while (true)
    {
        codec = av_codec_next(codec);
        if (codec == NULL)
        {
            break;
        }
        SYNC_PRINT(("0x%04.4X %s %s\n", codec->id, codec->name, codec->long_name));
    }
    SYNC_PRINT(("=============\n"));

}
#endif


void AVEncoder::rgb24BufferToAVFrameYUV420P(corecvs::RGB24Buffer *input, AVFrame *frame)
{
    /* Y */
    for(int  y = 0; y < frame->height; y++) {
        for(int x = 0; x < frame->width; x++) {
            frame->data[0][y * frame->linesize[0] + x] = input->element(y,x).y();
        }
    }

    /* Cb and Cr */
    for(int y = 0; y < frame->height / 2; y++) {
         for(int x = 0; x < frame->width / 2; x++) {
             int x2 = x * 2;
             int y2 = y * 2;

             int cb = input->element(y2    , x2    ).u() +
                      input->element(y2 + 1, x2    ).u() +
                      input->element(y2    , x2 + 1).u() +
                      input->element(y2 + 1, x2 + 1).u();
             cb = cb / 4;

             int cr = input->element(y2    , x2    ).v() +
                      input->element(y2 + 1, x2    ).v() +
                      input->element(y2    , x2 + 1).v() +
                      input->element(y2 + 1, x2 + 1).v();
             cr = cr / 4;

             frame->data[1][y * frame->linesize[1] + x] = cr;
             frame->data[2][y * frame->linesize[2] + x] = cb;
         }
     }
}

void AVEncoder::rgb24BufferToAVFrameBGR8(corecvs::RGB24Buffer *input, AVFrame *frame)
{
    /* BGR8 */
    for(int  y = 0; y < frame->height; y++) {
        for(int x = 0; x < frame->width; x++) {
            corecvs::RGBColor c = input->element(y,x);
            uint8_t v =
                ((c.r() & 0xE0)     ) |
                ((c.g() & 0xE0) >> 3) |
                ((c.b() & 0xE0) >> 6);

            frame->data[0][y * frame->linesize[0] + x] = v;
        }
    }
}
