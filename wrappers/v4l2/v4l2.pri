isEmpty(AVCODEC_WRAPPER_DIR) {
    message(Incorrect usage of avcodec.pri with empty AVCODEC_WRAPPER_DIR. AVCodec is switched off!)
} else {
    include(avcodecLibs.pri)
}

contains(DEFINES, WITH_AVCODEC) {                    # if it's installed properly with found path for lib

    INCLUDEPATH += $$AVCODEC_WRAPPER_DIR

    HEADERS += \
        $$AVCODEC_WRAPPER_DIR/aviCapture.h \
        $$AVCODEC_WRAPPER_DIR/rtspCapture.h \

    SOURCES += \
        $$AVCODEC_WRAPPER_DIR/aviCapture.cpp \
        $$AVCODEC_WRAPPER_DIR/rtspCapture.cpp \
}

