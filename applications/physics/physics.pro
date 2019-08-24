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
INCLUDEPATH += autopilot
INCLUDEPATH += copter
INCLUDEPATH += calibration
INCLUDEPATH += opencvUtils
INCLUDEPATH += simulation

HEADERS += \
    calibration/calibration.h \
    calibration/calibrationWidget.h \
    calibration/imageForCalibrationWidget.h \
    joystick/joystickreader.h \
    simulation/physMainObject.h \
    simulation/simulation.h \
    simulation/simSphere.h \
    simulation/simObject.h \
    clientSender.h \
    joystickInput.h \
    qComController.h \
    controlRecord.h \
    joystick/joystickInterface.h \
    joystick/JoystickOptionsWidget.h \
    radio/frSkyMultimodule.h \
    simulation/mainObject.h \
    copterInputsWidget.h \
    mixer/controlsMixer.h \
    copterInputs.h \
    joystick/mixerChannelOperationWidget.h \
    frameProcessor.h \
    copter/quad.h \
    physicsMainWindow.h \
    physicsAboutWidget.h \
    autopilot/protoautopilot.h \
    comcontroller.h \
    autopilot/vertexsquare.h\
    simulation/physObject.h \
    simulation/physSphere.h \
    copter/droneObject.h \
    copter/motor.h \
    opencvUtils/opencvTransformations.h


SOURCES += \
    calibration/calibration.cpp \
    calibration/calibrationWidget.cpp \
    calibration/imageForCalibrationWidget.cpp \
    joystick/joystickreader.cpp \
    simulation/physMainObject.cpp \
    simulation/simulation.cpp \
    simulation/simSphere.cpp \
    simulation/simObject.cpp \
    qComController.cpp \
    joystickInput.cpp \
    clientSender.cpp \
    mainPhysics.cpp \
    controlRecord.cpp \  
    joystick/joystickInterface.cpp \
    joystick/JoystickOptionsWidget.cpp \
    radio/frSkyMultimodule.cpp \
    simulation/mainObject.cpp \
    copterInputsWidget.cpp \
    mixer/controlsMixer.cpp \
    copterInputs.cpp \
    joystick/mixerChannelOperationWidget.cpp \
    frameProcessor.cpp \
    copter/quad.cpp \
    physicsMainWindow.cpp \
    physicsAboutWidget.cpp \
    autopilot/protoautopilot.cpp \
    comcontroller.cpp \
    autopilot/vertexsquare.cpp\
    simulation/physObject.cpp \
    simulation/physSphere.cpp \
    copter/droneObject.cpp \
    copter/motor.cpp \
    opencvUtils/opencvTransformations.cpp


FORMS += \
    calibration/calibrationWidget.ui \
    joystick/JoystickOptionsWidget.ui \
    copterInputsWidget.ui \
    calibration/imageForCalibrationWidget.ui \
    joystick/mixerChannelOperationWidget.ui \
    physicsMainWindow.ui \
    physicsAboutWidget.ui


LIBS+=  -lopencv_core\
        -lopencv_imgproc\
        -lopencv_imgcodecs\
        -lopencv_highgui\
        -lopencv_flann\
        -lopencv_calib3d\
        -lopencv_features2d\
        -lopencv_xfeatures2d

#RESOURCES += ../../resources/main.qrc

#HEADERS += xml/generated/*.h
#SOURCES += xml/generated/*.cpp

OTHER_FILES += xml/physics.xml
OTHER_FILES += ../../tools/generator/regen-physics.sh


