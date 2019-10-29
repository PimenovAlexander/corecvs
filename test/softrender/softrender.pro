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
TARGET   = softrender
CONFIG  += console
QT      += core

#include(../../core/core.pri)
include(../../utils/utils.pri)                      # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!


SOURCES += \
    main_softrender.cpp

with_libjpeg {
    LIBJPEG_WRAPPER_DIR = ../../wrappers/libjpeg
    include($$LIBJPEG_WRAPPER_DIR/libjpeg.pri)

    contains(DEFINES, WITH_LIBJPEG) {
        INCLUDEPATH += $$LIBJPEG_WRAPPER_DIR
    }
}

with_libpng {
    message(We have libpng)
    LIBPNG_WRAPPER_DIR = ../../wrappers/libpng
    include($$LIBPNG_WRAPPER_DIR/libpng.pri)
    contains(DEFINES, WITH_LIBPNG) {
        INCLUDEPATH += $$LIBPNG_WRAPPER_DIR
    }

}

OTHER_FILES += CMakeLists.txt
OTHER_FILES += render.sh
