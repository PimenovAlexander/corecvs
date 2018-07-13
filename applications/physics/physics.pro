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

INCLUDEPATH += .

HEADERS += \
    physicsMainWidget.h

SOURCES += \
    main_physics.cpp \
    physicsMainWidget.cpp

FORMS += \
    physicsMainWidget.ui

#RESOURCES += ../../resources/main.qrc
