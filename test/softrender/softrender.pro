# try use global config
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    #message(Using global config)
    include($$ROOT_DIR/config.pri)
} else {
    message(Using local config)
    ROOT_DIR=../..
    include($$ROOT_DIR/cvs-config.pri)
}
ROOT_DIR=$$PWD/$$ROOT_DIR

QT       += core

TEMPLATE = app
TARGET   = softrender
CONFIG  += console

TEST_DIR = $$PWD
#TEST_DIR = .
#message (Original PWD $$PWD  $$TEST_DIR)
UTILSDIR = $$TEST_DIR/../../utils
include($$UTILSDIR/utils.pri)


TARGET_ORIG = $$TARGET
TARGET      = $$join(TARGET,,,$$BUILD_CFG_SFX)  # add 'd' at the end for debug versions

OBJECTS_DIR = $$ROOT_DIR/.obj/$$TARGET_ORIG$$BUILD_CFG_NAME
MOC_DIR  = $$OBJECTS_DIR
UI_DIR   = $$OBJECTS_DIR
RCC_DIR  = $$OBJECTS_DIR

DESTDIR  = $$ROOT_DIR/bin

SOURCES += \
    main_softrender.cpp

