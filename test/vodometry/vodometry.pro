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

TEMPLATE = app
TARGET   = test_vodometry

with_libpng {
    LIBPNG_WRAPPER_DIR = ../../wrappers/libpng
    include($$LIBPNG_WRAPPER_DIR/libpng.pri)
}

include(../../utils/utils.pri)                      # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!


SOURCES += main.cpp \
    convertors.cpp \
    ertfinder.cpp

HEADERS += \
    convertors.h \
    ertfinder.h
