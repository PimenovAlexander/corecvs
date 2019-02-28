# try use global config
exists(../../../../config.pri) {
    #message(Using global config)
    ROOT_DIR=../../../..
    include($$ROOT_DIR/config.pri)
} else {
    message(Using local config)
    ROOT_DIR=../..
    include($$ROOT_DIR/cvs-config.pri)
}

DESTDIR = $$ROOT_DIR/bin
OBJECTS_DIR = $$ROOT_DIR/.obj/$$TARGET$$BUILD_CFG_NAME
MOC_DIR  = $$OBJECTS_DIR
UI_DIR   = $$OBJECTS_DIR
RCC_DIR  = $$OBJECTS_DIR

TEMPLATE=app
TARGET=test_grab24

TEST_DIR = $$PWD
include($$PWD/../../core/core.pri)

CONFIG += with_v4l2

with_v4l2 {
    V4L2_WRAPPER_DIR = $$PWD/../../wrappers/v4l2
    include($$V4L2_WRAPPER_DIR/v4l2.pri)                # it uses OPENCV_WRAPPER_DIR inside
}

with_avcodec {
    !build_pass: message(Switching on avcodec support)
    AVCODEC_WRAPPER_DIR = $$PWD/../../wrappers/avcodec
    include($$AVCODEC_WRAPPER_DIR/avcodec.pri)
}



SOURCES += main_grab24.cpp

HEADERS +=
