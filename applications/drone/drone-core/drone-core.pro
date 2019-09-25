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

TEMPLATE = lib
TARGET   = drone-core
CONFIG  += staticlib

include(../../../core/core.pri)                      # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!
include(drone-core.pri)

QT += serialport

HEADERS += \
    calibration/calibration.h \
    \
    copter/pid.h \
    copter/droneObject.h \
    copter/motor.h \
    copter/quad.h \
    \
    joystick/joystickInterface.h \
    \
    simulation/janibekovsBolt.h \
    simulation/physMainObject.h \
    simulation/simulation.h \
    simulation/simSphere.h \
    simulation/simObject.h \
    simulation/mainObject.h \
    simulation/physObject.h \
    simulation/physSphere.h \
    \
    mixer/controlsMixer.h \
    \
    radio/frSkyMultimodule.h \
    radio/multimoduleController.h \
    radio/r9Module.h \
    \
    autopilot/protoautopilot.h \
    autopilot/vertexsquare.h \
    opencvUtils/opencvTransformations.h \
    \
    clientSender.h \
    joystickInput.h \
    controlRecord.h \
    comcontroller.h \  
    copterInputs.h \
    joystick/joystickReader.h


SOURCES += \
    calibration/calibration.cpp \
    \
    copter/pid.cpp \
    copter/quad.cpp \
    copter/droneObject.cpp \
    copter/motor.cpp \
    \
    joystick/joystickInterface.cpp \
    \
    simulation/physMainObject.cpp \
    simulation/simulation.cpp \
    simulation/simSphere.cpp \
    simulation/simObject.cpp \
    simulation/mainObject.cpp \
    simulation/physObject.cpp \
    simulation/physSphere.cpp \
    simulation/dzhanibekovBolt.cpp \
    \
    mixer/controlsMixer.cpp \
    \
    radio/frSkyMultimodule.cpp \
    radio/multimoduleController.cpp \
    radio/r9Module.cpp \
    \
    autopilot/protoautopilot.cpp \
    autopilot/vertexsquare.cpp \
    opencvUtils/opencvTransformations.cpp \
    \
    joystickInput.cpp \
    clientSender.cpp \   
    controlRecord.cpp \
    copterInputs.cpp \   
    comcontroller.cpp \
    joystick/joystickReader.cpp


#RESOURCES += ../../resources/main.qrc

HEADERS += xml/generated/*.h
SOURCES += xml/generated/*.cpp

OTHER_FILES += xml/physics.xml
OTHER_FILES += ../../tools/generator/regen-physics.sh


