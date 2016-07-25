# try use global config
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    #message(Using global config)
    ROOT_DIR=$$PWD/$$ROOT_DIR
    include($$ROOT_DIR/config.pri)
} else {
    message(Using local config)
    ROOT_DIR=../..
    ROOT_DIR=$$PWD/$$ROOT_DIR
    include($$ROOT_DIR/cvs-config.pri)
}

QT += core
QT -= gui

TARGET = laserscan
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

include($$ROOT_DIR/utils/utils.pri)                        # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!


SOURCES += main.cpp

HEADERS +=

