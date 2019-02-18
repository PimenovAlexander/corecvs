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

HEADERS += \
    physicsMainWidget.h \
    clientsender.h \
    controllrecord.h \
    joystickinput.h \
    qcomcontroller.h \
<<<<<<< HEAD
    simulation.h
=======
    simobject.h
>>>>>>> 06a4b96fb5813ddafed697eb061ab707c7a8bca8

SOURCES += \
    main_physics.cpp \
    physicsMainWidget.cpp \
    clientsender.cpp \
    controllrecord.cpp \
    joystickinput.cpp \
    qcomcontroller.cpp \
<<<<<<< HEAD
    simulation.cpp
=======
    simobject.cpp
>>>>>>> 06a4b96fb5813ddafed697eb061ab707c7a8bca8

FORMS += \
    physicsMainWidget.ui

#RESOURCES += ../../resources/main.qrc

