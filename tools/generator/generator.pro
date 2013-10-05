QT += core xml
QT -= gui

TARGET  = generator
CONFIG += console
CONFIG -= app_bundle
CONFIG += debug

TEMPLATE = app

include(../../../../config.pri)

win32 {
    OBJECTS_DIR = ../../../../.obj/generator/$$BUILD_CFG_NAME
} else {
    OBJECTS_DIR = ../../../../.obj/generator
}
MOC_DIR  = $$OBJECTS_DIR
#UI_DIR  = $$OBJECTS_DIR
#RCC_DIR = $$OBJECTS_DIR

COREDIR = ../../core
include($$COREDIR/core.pri)

SOURCES += \
    main.cpp \
#    example.cpp \
    pdoGenerator.cpp \
    widgetUIGenerator.cpp \
    baseGenerator.cpp \
#    qSettingsVisitors.cpp \
    parametersMapperGenerator.cpp \
    configLoader.cpp

HEADERS += \
    reflectionGenerator.h \
#    example.h \
    pdoGenerator.h \
    widgetUIGenerator.h \
    baseGenerator.h \
#    qSettingsVisitors.h \
    parametersMapperGenerator.h \
    configLoader.h

#SOURCES += Generated/Example.cpp
#HEADERS += Generated/Example.h
