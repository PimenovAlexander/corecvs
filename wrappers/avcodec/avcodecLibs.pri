with_avcodec {


    win32 {
        isEmpty(AVCODEC_PATH): AVCODEC_PATH = "c:/ffmpeg"
        !build_pass: message(AvCodec $$AVCODEC_PATH)

        DEFINES     += WITH_AVCODEC
        INCLUDEPATH += $$AVCODEC_PATH/include
        LIBS        += -L$$AVCODEC_PATH/lib -lavutil -lavformat -lavcodec -lavutil -lm
    } else {
        system(pkg-config libavcodec libavutil libavformat libswscale) {
            DEFINES += WITH_AVCODEC
            LIBS    += -lavutil -lavformat -lavcodec -lz -lavutil -lm

            !build_pass: message(Switching on AVCodec support)
            !build_pass: message(Switching on swscale support)

            DEFINES += WITH_SWSCALE
            LIBS    += -lswscale -lswresample
        } else {
            !build_pass: message(AVCodec support requested but not found)
        }
    }
}
