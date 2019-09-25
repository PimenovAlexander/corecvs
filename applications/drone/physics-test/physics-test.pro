# try use global config
exists(../../../../../config.pri) {
    ROOT_DIR=../../../../..
    #message(Using global config)
    include($$ROOT_DIR/config.pri)
} else {
    #message(Using local config)
    ROOT_DIR=../../..
    include($$ROOT_DIR/cvs-config.pri)
}
ROOT_DIR=$$PWD/$$ROOT_DIR

DESTDIR = $$ROOT_DIR/bin
OBJECTS_DIR = $$ROOT_DIR/.obj/$$TARGET_ORIG$$BUILD_CFG_NAME

TEMPLATE = app
TARGET   = test_physics

include($$PWD/../../../wrappers/gtest/gtest.pri)
include($$PWD/../../../core/core.pri)

include($$PWD/../../../utils/utils.pri)
include($$PWD/../drone-core/drone-core.pri) # Drone core should be stripped from UI


#with_avcodec {
#    !build_pass: message(Switching on avcodec support)
#    !build_pass: message(Switching on swscale support)
#
#    AVCODEC_WRAPPER_DIR = ../../../wrappers/avcodec
#    include($$AVCODEC_WRAPPER_DIR/avcodec.pri)
#}

SOURCES += main_physics_test.cpp

LIBS +=  -ldrone-core -lcvs_utils -lcvs_core $$LIBS

