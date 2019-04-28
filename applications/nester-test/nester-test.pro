# try use global config
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    #message(Using global config)
    include($$ROOT_DIR/config.pri)
} else { 
    #message(Using local config)
    ROOT_DIR=../..
    include($$ROOT_DIR/cvs-config.pri)
}
ROOT_DIR=$$PWD/$$ROOT_DIR

DESTDIR = $$ROOT_DIR/bin
OBJECTS_DIR = $$ROOT_DIR/.obj/$$TARGET_ORIG$$BUILD_CFG_NAME

TEMPLATE = app
TARGET   = test_nester

include($$PWD/../../wrappers/gtest/gtest.pri)
include($$PWD/../../core/core.pri)

with_libjpeg {
    LIBJPEG_WRAPPER_DIR = ../../wrappers/libjpeg
    include($$LIBJPEG_WRAPPER_DIR/libjpeg.pri)
}

with_libpng {
    LIBPNG_WRAPPER_DIR = ../../wrappers/libpng
    include($$LIBPNG_WRAPPER_DIR/libpng.pri)
}

SOURCES += ../nester/nester.cpp
HEADERS += ../nester/nester.h

SOURCES += main_nester_test.cpp

