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
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


include($$ROOT_DIR/core/core.pri)

SOURCES += main.cpp

with_libjpeg {                                       # all this stuff was extracted from opencv.pri to speedup including
    LIBJPEG_WRAPPER_DIR = $$ROOT_DIR/wrappers/libjpeg
    include($$LIBJPEG_WRAPPER_DIR/libjpeg.pri)

    contains(DEFINES, WITH_LIBJPEG) {
        INCLUDEPATH += $$LIBJPEG_WRAPPER_DIR
    }
}

