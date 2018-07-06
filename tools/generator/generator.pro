# try use global config
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    include($$ROOT_DIR/config.pri)
} else {
    !build_pass: message(Using local config in core.pro)
    ROOT_DIR=../..
    include($$ROOT_DIR/cvs-config.pri)
}

TEMPLATE = app
TARGET   = generator
QT      += core xml
QT      -= gui
CONFIG  += console
CONFIG  -= app_bundle
CONFIG  += debug
CONFIG  += c++11


OBJECTS_DIR = $$ROOT_DIR/.obj/generator$$BUILD_CFG_NAME
MOC_DIR  = $$OBJECTS_DIR
#UI_DIR  = $$OBJECTS_DIR
#RCC_DIR = $$OBJECTS_DIR

# CORE SUBSET
COREDIR=../../core

INCLUDEPATH += \
    $$COREDIR/.. \
    $$COREDIR/reflection \
    $$COREDIR/utils   \

SOURCES += $$COREDIR/utils/util.c
SOURCES += $$COREDIR/utils/utils.cpp

!win32  {
    LIBS += -lstdc++fs
}


# ACTUAL GENERATOR


SOURCES += \
    main.cpp \
#    example.cpp \
    pdoGenerator.cpp \
    widgetUIGenerator.cpp \
    documentationGenerator.cpp \
    baseGenerator.cpp \
#    qSettingsVisitors.cpp \
    parametersMapperGenerator.cpp \
    configLoader.cpp

HEADERS += \
    reflectionGenerator.h \
#    example.h \
    pdoGenerator.h \
    widgetUIGenerator.h \
    documentationGenerator.h \
    baseGenerator.h \
#    qSettingsVisitors.h \
    parametersMapperGenerator.h \
    configLoader.h

OTHER_FILES += \
    xml/test.xml \
    selftest.sh  \


#SOURCES += Generated/Example.cpp
#HEADERS += Generated/Example.h
