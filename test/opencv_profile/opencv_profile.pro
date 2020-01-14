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


TEMPLATE=app
TARGET=opencv_profile

TEST_DIR = $$PWD
#TEST_DIR = .
#message (Original PWD $$PWD  $$TEST_DIR)
UTILSDIR = $$TEST_DIR/../../utils

include($$TEST_DIR/../../wrappers/gtest/gtest.pri)
include($$UTILSDIR/utils.pri)


SOURCES += \   
    main.cpp

