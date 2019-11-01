# try use global config
exists(../../../../config.pri) {
    #message(Using global config)
    ROOT_DIR=../../../..
    include($$ROOT_DIR/config.pri)
} else {
    #message(Using local config)
    ROOT_DIR=../..
    include($$ROOT_DIR/cvs-config.pri)
}

DESTDIR = $$ROOT_DIR/bin
OBJECTS_DIR = $$ROOT_DIR/.obj/$$TARGET$$BUILD_CFG_NAME
MOC_DIR  = $$OBJECTS_DIR
UI_DIR   = $$OBJECTS_DIR
RCC_DIR  = $$OBJECTS_DIR

TEMPLATE = app
CONFIG  += console c++11
CONFIG  -= app_bundle
CONFIG  -= qt

include($$PWD/../../core/core.pri)

QT -= gui

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

with_libjpeg {
    LIBJPEG_WRAPPER_DIR = ../../wrappers/libjpeg
    include($$LIBJPEG_WRAPPER_DIR/libjpeg.pri)
}

with_libpng {
    LIBPNG_WRAPPER_DIR = ../../wrappers/libpng
    include($$LIBPNG_WRAPPER_DIR/libpng.pri)
}

SOURCES += \
        imageStack.cpp \
        laplacianStacking.cpp \
        main.cpp

HEADERS += \
    FSAlgorithm.h \
    imageStack.h \
    laplacianStacking.h

OTHER_FILES += CMakeLists.txt
