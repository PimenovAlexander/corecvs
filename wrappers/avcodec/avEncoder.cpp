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
    //av_register_all();
    //avcodec_register_all();

    if (codec_id == -1) {
        codec_id = AV_CODEC_ID_MPEG4;
    }


    /* allocate the output media context */
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


     /* find video encoder */
     codec = avcodec_find_encoder((AVCodecID)codec_id);
     if (codec == NULL) {
         fprintf(stderr, "Codec not found\n");
         return 1;
     }

     outStream = avformat_new_stream(formatContext, codec);
     if (outStream == NULL) {
         fprintf(stderr, "Could not allocate stream\n");
         return 1;
     }

     outStream->id = formatContext->nb_streams - 1;
     SYNC_PRINT(("Stream id %d\n", outStream->id));

     codecContext = outStream->codec;
     codecContext->codec_id = codec->id;

     if (codecContext == NULL) {
         fprintf(stderr, "Could not allocate video codec context\n");
         return 1;
     }

     /* put sample parameters */
     codecContext->bit_rate = 12400000;
     /* resolution must be a multiple of two */
     codecContext->width = w;
     codecContext->height = h;
     /* frames per second */
     codecContext->time_base= AVRational({1,25});
     codecContext->gop_size = 10; /* emit one intra frame every ten frames */
     codecContext->max_b_frames=1;
     codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

     if(codec_id == AV_CODEC_ID_H264) {
         av_opt_set(codecContext->priv_data, "preset", "slow", 0);
     }

     /* Muxing */
     av_dump_format(formatContext, 0, name.c_str(), 1);

     if (!(outputFormat->flags & AVFMT_NOFILE)) {
         ret = avio_open(&formatContext->pb, name.c_str(), AVIO_FLAG_WRITE);
         if (ret < 0) {
             fprintf(stderr, "Could not open '%s': %d\n", name.c_str(), ret);
             return 1;
         }
     }


     /* open codec */
     if (avcodec_open2(codecContext, codec, NULL) < 0) {
         fprintf(stderr, "Could not open codec\n");
         return 1;
     }

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
     ret = av_image_alloc(frame->data, frame->linesize, codecContext->width, codecContext->height,
                          codecContext->pix_fmt, 32);
     if (ret < 0) {
         fprintf(stderr, "Could not allocate raw picture buffer\n");
         exit(1);
     }


     SYNC_PRINT(("Writing header\n"));
     ret = avformat_write_header(formatContext, NULL);
     if (ret < 0) {
         SYNC_PRINT(("Writing format problem"));
         return ret;
     }

     open = true;
     return 0;
}

void AVEncoder::addFrame(corecvs::RGB24Buffer *input)
{
    int ret;

    if (input == NULL)
        return;

    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;    // packet data will be allocated by the encoder
    pkt.size = 0;

     fflush(stdout);
     /* prepare a dummy image */
     /* Y */
     for(int  y = 0; y < codecContext->height; y++) {
         for(int x = 0; x < codecContext->width; x++) {
             frame->data[0][y * frame->linesize[0] + x] = input->element(y,x).y();
         }
     }

     /* Cb and Cr */
     for(int y = 0; y < codecContext->height / 2; y++) {
         for(int x = 0; x < codecContext->width / 2; x++) {
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

     frame->pts = frame_number;

     /* encode the image */
     ret = avcodec_encode_video2(codecContext, &pkt, frame, &got_output);
     if (ret < 0) {
         fprintf(stderr, "Error encoding frame\n");
         exit(1);
     }

     if (got_output) {
         printf("Write frame %3d (size=%5d)\n", frame_number, pkt.size);
         fwrite(pkt.data, 1, pkt.size, outFile);
         av_packet_unref(&pkt);
     }

     /* get the delayed frames */
     for (got_output = 1; got_output; frame_number++) {
         fflush(stdout);

         ret = avcodec_encode_video2(codecContext, &pkt, NULL, &got_output);
         if (ret < 0) {
             fprintf(stderr, "Error encoding frame\n");
             return;
         }

         if (got_output) {
             printf("Write frame %3d (size=%5d)\n", frame_number, pkt.size);
             //fwrite(pkt.data, 1, pkt.size, outFile);
             if ((ret = av_interleaved_write_frame(formatContext, &pkt)) < 0) {
                 fprintf(stderr, "Failed to write packet\n");
                 return;
             }
         }
         av_packet_unref(&pkt);

     }

}

void AVEncoder::endEncoding()
{
    //int ret;
    //uint8_t endcode[] = { 0, 0, 1, 0xb7 };



    /* add sequence end code to have a real mpeg file */
    //fwrite(endcode, 1, sizeof(endcode), outFile);
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

#if 0
void AVEncoder::printCaps()
{
    //av_register_all();
    //avcodec_register_all();

    SYNC_PRINT(("AVEncoder::printCaps():format list\n"));
    void **opaque = NULL;

    const AVOutputFormat * oformat = av_muxer_iterate(opaque);
    while(oformat != NULL)
    {
        SYNC_PRINT(("%s %s\n", oformat->name, oformat->long_name));
        oformat = av_muxer_iterate(opaque);
    }

    SYNC_PRINT(("=============\n"));

    SYNC_PRINT(("AVEncoder::printCaps():codec list\n"));

    opaque = NULL;
    const AVCodec *codec = NULL;
    while (true)
    {
        codec = av_codec_iterate(opaque);
        if (codec == NULL)
        {
            break;
        }
        SYNC_PRINT(("0x%04.4X %s %s\n", codec->id, codec->name, codec->long_name));
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
