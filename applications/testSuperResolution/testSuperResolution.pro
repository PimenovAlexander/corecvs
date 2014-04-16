# try use global config
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
} else {
    message(Using local config)
    ROOT_DIR=../..
}
ROOT_DIR=$$PWD/$$ROOT_DIR
include($$ROOT_DIR/config.pri)

QT += xml
TARGET   = testSuperResolution
TEMPLATE = app

#OBJECTS_DIR = $$ROOT_DIR/.obj
#MOC_DIR = $$ROOT_DIR/.obj
#UI_DIR  = $$ROOT_DIR/.obj

# Using only open part of the project
UTILSDIR = ../../utils
include($$UTILSDIR/utils.pri)

win32 {
    MOC_DIR = ../../../../.obj/$$TARGET_ORIG/$$BUILD_CFG_NAME  # resolve moc path for mocs to help qmake to unify those paths.
}

HEADERS = \
    testSuperResolutionMainWindow.h \
#    pointScene.h \
    resamples.h \
    convolution.h \
    modelingProcess.h \
    transformations.h \
    gradientDescent.h \
    commonStructures.h \
    polygons.h


SOURCES = \
    testSuperResolutionMainWindow.cpp \
    main_testSuperResolution.cpp \
#    pointScene.cpp \
    resamples.cpp \
    convolution.cpp \
    modelingProcess.cpp \
    transformations.cpp \
    gradientDescent.cpp \
    commonStructures.cpp \
    polygons.cpp
FORMS = ui/testSuperResolutionMainWindow.ui \

RESOURCES += ../../resources/main.qrc


QMAKE_CXXFLAGS += -std=c++11
