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
TARGET   = test_physics

include($$PWD/../../wrappers/gtest/gtest.pri)
include($$PWD/../../core/core.pri)

with_avcodec {
    !build_pass: message(Switching on avcodec support)
    !build_pass: message(Switching on swscale support)

    AVCODEC_WRAPPER_DIR = ../../wrappers/avcodec
    include($$AVCODEC_WRAPPER_DIR/avcodec.pri)
}

SOURCES += main_physics_test.cpp

##
INCLUDEPATH += ../physics
INCLUDEPATH += ../physics/radio
INCLUDEPATH += ../physics/simulation

HEADERS += ../physics/radio/frSkyMultimodule.h
SOURCES += ../physics/radio/frSkyMultimodule.cpp
HEADERS += ../physics/radio/multimoduleController.h
SOURCES += ../physics/radio/multimoduleController.cpp

HEADERS += ../physics/copter/quad.h
SOURCES += ../physics/copter/quad.cpp

HEADERS += ../physics/copter/quadAngles.h
SOURCES += ../physics/copter/quadAngles.cpp

HEADERS += ../physics/simulation/simObject.h
SOURCES += ../physics/simulation/simObject.cpp

HEADERS += ../physics/copterInputs.h
SOURCES += ../physics/copterInputs.cpp

INCLUDEPATH += ../physics/xml/generated
HEADERS += ../physics/xml/generated/*.h
SOURCES += ../physics/xml/generated/*.cpp

