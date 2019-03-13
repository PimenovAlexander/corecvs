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

TEMPLATE = app
TARGET   = physics

include(../../utils/utils.pri)                      # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!

QT += serialport

INCLUDEPATH += .
INCLUDEPATH += joystick
INCLUDEPATH += radio
INCLUDEPATH += mixer


HEADERS += \
    physicsMainWidget.h \
    simulation.h \
    clientSender.h \
    joystickInput.h \
    qComController.h \
    simObject.h \
    simSphere.h \
    controlRecord.h \
    joystick/joystickInterface.h \
    joystick/JoystickOptionsWidget.h \
    radio/frSkyMultimodule.h \
    mainObject.h \
    copterInputsWidget.h \
    mixer/controlsMixer.h \
    copterInputs.h \
    joystick/mixerChannelOperationWidget.h \
    frameProcessor.h \
    protoautopilot.h \
    copter/quad.h

SOURCES += \
    physicsMainWidget.cpp \
    simulation.cpp \
    simSphere.cpp \
    simObject.cpp \
    qComController.cpp \
    joystickInput.cpp \
    clientSender.cpp \
    mainPhysics.cpp \
    controlRecord.cpp \  
    joystick/joystickInterface.cpp \
    joystick/JoystickOptionsWidget.cpp \
    radio/frSkyMultimodule.cpp \
    mainObject.cpp \
    copterInputsWidget.cpp \
    mixer/controlsMixer.cpp \
    copterInputs.cpp \
    joystick/mixerChannelOperationWidget.cpp \
    frameProcessor.cpp \
    protoautopilot.cpp \
    copter/quad.cpp

FORMS += \
    physicsMainWidget.ui \
    joystick/JoystickOptionsWidget.ui \
    copterInputsWidget.ui \
    joystick/mixerChannelOperationWidget.ui

#RESOURCES += ../../resources/main.qrc

#HEADERS += xml/generated/*.h
#SOURCES += xml/generated/*.cpp

OTHER_FILES += xml/physics.xml
OTHER_FILES += ../../tools/generator/regen-physics.sh


