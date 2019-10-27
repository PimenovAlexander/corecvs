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
OBJECTS_DIR = $$ROOT_DIR/.obj/$$TARGET$$BUILD_CFG_NAME
MOC_DIR  = $$OBJECTS_DIR
UI_DIR   = $$OBJECTS_DIR
RCC_DIR  = $$OBJECTS_DIR
TEMPLATE = lib
TARGET   = drone-ui
CONFIG  += staticlib

include(../../../utils/utils.pri)                      # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!
include(../drone-core/drone-core.pri)
include(drone-ui.pri)


QT += serialport

HEADERS += \
    calibration/calibrationWidget.h \
    calibration/imageForCalibrationWidget.h \
    \
    joystick/JoystickOptionsWidget.h \
    joystick/mixerChannelOperationWidget.h \
    \
    radio/radioControlWidget.h \
    copterInputsWidget.h \
    \
    physicsMainWindow.h \
    physicsAboutWidget.h \


SOURCES += \
    calibration/calibrationWidget.cpp \
    calibration/imageForCalibrationWidget.cpp \
    \
    joystick/JoystickOptionsWidget.cpp \
    joystick/mixerChannelOperationWidget.cpp \
    \
    radio/radioControlWidget.cpp \
    copterInputsWidget.cpp \   
    \
    physicsMainWindow.cpp \
    physicsAboutWidget.cpp \   

FORMS += \
    calibration/calibrationWidget.ui \
    calibration/imageForCalibrationWidget.ui \
    joystick/JoystickOptionsWidget.ui \
    joystick/mixerChannelOperationWidget.ui \
    radio/radioControlWidget.ui \
    copterInputsWidget.ui \
    physicsMainWindow.ui \
    physicsAboutWidget.ui


SOURCES += frameProcessor.cpp
HEADERS += frameProcessor.h


#RESOURCES += ../../resources/main.qrc


