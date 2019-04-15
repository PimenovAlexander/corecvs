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
    joystick/joystickInterface.h \
    joystick/JoystickOptionsWidget.h \
    joystick/mixerChannelOperationWidget.h \
    mixer/controlsMixer.h \
    radio/*.h \
    copter/quad.h \
    simulation.h \
    clientSender.h \
    joystickInput.h \
    simObject.h \
    simSphere.h \
    controlRecord.h \
    mainObject.h \
    copterInputsWidget.h \
    copterInputs.h \
    frameProcessor.h \
    protoautopilot.h \   
    physicsMainWindow.h \
    physicsAboutWidget.h \

SOURCES += \
    joystick/joystickInterface.cpp \
    joystick/JoystickOptionsWidget.cpp \
    joystick/mixerChannelOperationWidget.cpp \
    mixer/controlsMixer.cpp \
    copter/quad.cpp \
    radio/*.cpp \
    simulation.cpp \
    simSphere.cpp \
    simObject.cpp \
    joystickInput.cpp \
    clientSender.cpp \
    mainPhysics.cpp \
    controlRecord.cpp \     
    mainObject.cpp \
    copterInputsWidget.cpp \
    copterInputs.cpp \
    frameProcessor.cpp \
    protoautopilot.cpp \    
    physicsMainWindow.cpp \
    physicsAboutWidget.cpp \

FORMS += \
    joystick/JoystickOptionsWidget.ui \
    copterInputsWidget.ui \
    joystick/mixerChannelOperationWidget.ui \
    physicsMainWindow.ui \
    physicsAboutWidget.ui \
    radio/radioControlWidget.ui

#RESOURCES += ../../resources/main.qrc

#HEADERS += xml/generated/*.h
#SOURCES += xml/generated/*.cpp

OTHER_FILES += xml/physics.xml
OTHER_FILES += ../../tools/generator/regen-physics.sh


