#
# Each Test/Tool uses this internal file
#
# Input parameter: OBJECTS_DIRNAME_TESTS - name of the common intermediate dir for all Tests of the current project
#
# Note:  $$PWD - returns the folder, where this file is placed,
#                despite on how it was included (e.g. include "../test.pri").
#

# try use global config 
exists(../../../config.pri) {
    ROOT_DIR=../../..
    include($$ROOT_DIR/config.pri)
} else {
    message(Using local config at test.pri for the $$TARGET)
    ROOT_DIR=..
    include($$ROOT_DIR/cvs-config.pri)
}
ROOT_DIR=$$PWD/$$ROOT_DIR

TEMPLATE = app
CONFIG  += console
CONFIG  -= app_bundle
QT      -= core
QT      -= gui
QT      -= xml
QT      -= widgets
QT      -= opengl

DESTDIR = $$ROOT_DIR/bin

SUPPRESSINCLUDES=true
include(../core/core.pri)
include(../wrappers/gtest/gtest.pri)
include(../wrappers/eigen/eigen.pri)

isEmpty(TARGET_ORIG) {                                      # be careful of multiple including of this file
    TARGET_ORIG = $$TARGET                                  # store original target name for proper detection of the obj.dir
    !contains(OBJECTS_DIRNAME_TESTS, cvs_tests_restricted) {# first include file is "testsCommon.pri" (open tests), second - "testsRestricted.pri" (restricted tests)
        TARGET = $$join(TARGET,,test_,)                     # use target name common format for all open tests as  "test_<name of the test>"
    } else {
        TARGET = $$join(TARGET,,cvs_test_res_,)             # use target name common format for all restricted tests as  "test_res_<name of the test>"
    }
    TARGET = $$join(TARGET,,,$$BUILD_CFG_SFX)               # add 'd' at the end for debug versions
}
isEmpty(OBJECTS_DIRNAME_TESTS) {
    OBJECTS_DIRNAME_TESTS = test_core                       # by default put current project's obj-files to this subfolder
}
OBJECTS_DIR = $$ROOT_DIR/.obj/$$OBJECTS_DIRNAME_TESTS/$$TARGET_ORIG$$BUILD_CFG_NAME

MOC_DIR = $$OBJECTS_DIR                                     # we have to set it to omit creating dummy dirs: debug,release
UI_DIR  = $$OBJECTS_DIR
RCC_DIR = $$OBJECTS_DIR
