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

QT       += core

TEMPLATE = app
TARGET   = test_widgets
CONFIG  += console

#include(../../core/core.pri)
include(../../utils/utils.pri)                      # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!

with_libjpeg {
    LIBJPEG_WRAPPER_DIR = ../../wrappers/libjpeg
    include($$LIBJPEG_WRAPPER_DIR/libjpeg.pri)
}

with_libpng {
    LIBPNG_WRAPPER_DIR = ../../wrappers/libpng
    include($$LIBPNG_WRAPPER_DIR/libpng.pri)
}

HEADERS += \
    changeReceiver.h \
    testNativeWidget.h

SOURCES += \
    main_widgets_test.cpp \
    testNativeWidget.cpp

GEN_FOLDER="../../tools/generator/Generated"
exists($$GEN_FOLDER/testClass.cpp) {
    message(Adding generator test)

    SOURCES += $$GEN_FOLDER/testSubClass.cpp
    SOURCES += $$GEN_FOLDER/testClass.cpp
    SOURCES += $$GEN_FOLDER/testBlock.cpp

    HEADERS += $$GEN_FOLDER/testSubClass.h
    HEADERS += $$GEN_FOLDER/testClass.h
    HEADERS += $$GEN_FOLDER/testBlock.h

    INCLUDEPATH += $$GEN_FOLDER
    DEPENDPATH  += $$GEN_FOLDER

    DEFINES += INCLUDE_EXAMPLE
}

FORMS += \
    testNativeWidget.ui \
