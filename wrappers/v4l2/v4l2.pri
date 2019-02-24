isEmpty(V4L2_WRAPPER_DIR) {
    message(Incorrect usage of avcodec.pri with empty V4L2_WRAPPER_DIR. V4L2 is switched off!)
} else {
    include(v4l2Libs.pri)
}

contains(DEFINES, WITH_V4L2) {                    # if it's installed properly with found path for lib

    message (Switching on V4L2 support)

    INCLUDEPATH += $$V4L2_WRAPPER_DIR

    HEADERS += \
        $$V4L2_WRAPPER_DIR/V4L2.h \
        $$V4L2_WRAPPER_DIR/V4L2Capture.h \
        $$V4L2_WRAPPER_DIR/V4L2CaptureDecouple.h \

    SOURCES += \
        $$V4L2_WRAPPER_DIR/V4L2.cpp \
        $$V4L2_WRAPPER_DIR/V4L2Capture.cpp \
        $$V4L2_WRAPPER_DIR/V4L2CaptureDecouple.cpp \
}
