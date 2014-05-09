# try use global config
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    #message(Using global config)
} else { 
    message(Using local config)
    ROOT_DIR=../..
}
ROOT_DIR=$$PWD/$$ROOT_DIR
#message(Tests root dir is $$ROOT_DIR)
include($$ROOT_DIR/config.pri)

TEMPLATE=app
TARGET=test_adoptcolor

TEST_DIR = $$PWD
#TEST_DIR = .
#message (Original PWD $$PWD  $$TEST_DIR)
UTILSDIR = $$TEST_DIR/../../utils
include($$UTILSDIR/utils.pri)


SOURCES += main_adoptcolor.cpp

HEADERS += main_adoptcolor.h
