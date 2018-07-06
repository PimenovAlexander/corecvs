with_avcodec {

    !build_pass: message(Switching on AVCodec support)

    win32 {
        isEmpty(AVCODEC_PATH): AVCODEC_PATH = "c:/ffmpeg"
        !build_pass: message(AvCodec $$AVCODEC_PATH)

        DEFINES     += WITH_AVCODEC
        INCLUDEPATH += $$AVCODEC_PATH/include
        LIBS        += -L$$AVCODEC_PATH/lib -lavutil -lavformat -lavcodec -lavutil -lm
    } else {
        DEFINES += WITH_AVCODEC
        LIBS    += -lavutil -lavformat -lavcodec -lz -lavutil -lm

        !build_pass: message(Switching on swscale support)

        DEFINES += WITH_SWSCALE
        LIBS    += -lswscale
    }
}
