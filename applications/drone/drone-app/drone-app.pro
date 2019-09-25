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

TEMPLATE = app
TARGET   = physics

#include(../../../utils/utils.pri)                      # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!
include(../drone-core/drone-core.pri)
include(../drone-ui/drone-ui.pri)

QT += serialport

SOURCES += mainDrone.cpp

LIBS +=  -ldrone-ui -ldrone-core -lcvs_utils -lcvs_core $$LIBS

