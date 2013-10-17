# This file is used internally for each UnitTest
#
# input1 parameter: $$OBJ_TESTS_DIR        - name of common intermediate dir for all UnitTests of the current project
# input2 parameter: $$USE_CORE_PRI_FILE    - required core|core-res project file to include
#

# try use global config 
exists(../../../config.pri) {
    ROOT_DIR=../../..
    #message(Using global config)
} else { 
    message(Using local config)
    ROOT_DIR=..
}
ROOT_DIR=$$PWD/$$ROOT_DIR
message(Tests root dir is $$ROOT_DIR)
include($$ROOT_DIR/config.pri)

CONFIG += console

win32-msvc* {
    # Sometimes mt.exe fails on embedding action in parallel making...
    #CONFIG -= embed_manifest_exe
}

DESTDIR = $$ROOT_DIR/bin

#macx {
#    INCLUDEDIR = ../core
#} else:gen_vsproj {
#    INCLUDEDIR = ../core                            # on generating VS projects all paths are accounting relatively to this pri-file dir
#} else {
#    INCLUDEDIR = ../../core
#}
#win32-msvc* {
#    COREDIR = ../../core                            # path to core includes from any of tests directory
#} else {
#    COREDIR = $$INCLUDEDIR
#}


#include($$INCLUDEDIR/$$USE_CORE_PRI_FILE)           # it uses COREDIR, TARGET and detects COREBINDIR|RES_COREBINDIR!
include($$USE_CORE_PRI_FILE)

#message(Using core $$USE_CORE_PRI_FILE  with $$COREDIR and $$CORERESDIR)

!contains(OBJ_TESTS_DIR, tests-res) {               # first include file is "testsCommon.pri", second - "testsRestricted.pri"
    TARGET_ORIG = $$TARGET                          # store original target name for proper detection of the obj.dir
    TARGET = $$join(TARGET,,test_,)                 # use target name common format for all tests as  "test_<name of the test>"

    # Note: debug and release libs will be overwritten on !win32 only
    #
    TARGET = $$join(TARGET,,,$$BUILD_CFG_SFX)       # add 'd' at the end for debug win32 version
}

win32 {
    OBJECTS_DIR = $$ROOT_DIR/.obj/$$OBJ_TESTS_DIR/$$TARGET_ORIG/$$BUILD_CFG_NAME
} else {
    OBJECTS_DIR = $$ROOT_DIR/.obj/$$OBJ_TESTS_DIR/$$TARGET_ORIG
}
MOC_DIR = $$OBJECTS_DIR                             # we have to set it to omit creating dummy dirs: debug,release
