##################################################################
# ocv-homo.pro created on Aug 13, 2011
# This is a file for QMAKE that allows to build the test ocv-homo
#
##################################################################
include(../testsCommon.pri)
#include(../testsRestricted.pri)

OPENCV_WRAPPER_DIR = ../../wrappers/opencv
include($$OPENCV_WRAPPER_DIR/opencv.pri)

INCLUDEPATH += ../../utils/corestructs/libWidgets

HEADERS += ../../utils/corestructs/libWidgets/openCVSGMParameters.h
HEADERS += ../../utils/corestructs/libWidgets/openCVBMParameters.h

SOURCES += ../../utils/corestructs/libWidgets/openCVSGMParameters.cpp
SOURCES += ../../utils/corestructs/libWidgets/openCVBMParameters.cpp

SOURCES += main_test_ocv-homo.cpp

