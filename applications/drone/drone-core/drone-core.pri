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

include(../../../utils/utils.pri)                      # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!

QT += serialport

INCLUDEPATH += $$PWD/.
INCLUDEPATH += $$PWD/joystick
INCLUDEPATH += $$PWD/radio
INCLUDEPATH += $$PWD/mixer
INCLUDEPATH += $$PWD/autopilot
INCLUDEPATH += $$PWD/copter
INCLUDEPATH += $$PWD/calibration
INCLUDEPATH += $$PWD/opencvUtils
INCLUDEPATH += $$PWD/simulation

