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
TARGET   = copter_control

HOSTBASE_DIR=../../base
include ($$HOSTBASE_DIR/baseApplication.pri)            # it uses HOSTBASE_DIR, detects HOSTBASE_BINDIR, OBJECTS_DIR, ...

INCLUDEPATH += .

HEADERS += \
    copterDialog.h \
    copterThread.h \
    copterControlWidget.h \                           # control widgets
    generatedParameters/copter.h \                    # parameters for calculation threads, host dialogs etc.
    parametersMapper/parametersMapperCopter.h \       # parameters for params mapper

SOURCES += \
    main_copter.cpp \
    copterDialog.cpp \
    copterThread.cpp \
    copterControlWidget.cpp \                         # control widgets
    generatedParameters/copter.cpp \                  # parameters for calculation threads, host dialogs etc.
    parametersMapper/parametersMapperCopter.cpp \     # parameters for params mapper

FORMS += \
    generatedParameters/copterControlWidget.ui \

#RESOURCES += ../../resources/main.qrc

OTHER_FILES += ../../tools/generator/xml/copter.xml
OTHER_FILES += ../../tools/generator/regen-copter.sh

