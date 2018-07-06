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
TARGET   = recorder

HOSTBASE_DIR=../base
include ($$HOSTBASE_DIR/baseApplication.pri)            # it uses HOSTBASE_DIR, detects HOSTBASE_BINDIR, OBJECTS_DIR, ...

INCLUDEPATH += .

HEADERS += \
    recorderDialog.h \
    recorderThread.h \
    recorderControlWidget.h \                           # control widgets
    generatedParameters/recorder.h \                    # parameters for calculation threads, host dialogs etc.
    parametersMapper/parametersMapperRecorder.h \       # parameters for params mapper

SOURCES += \
    main_recorder.cpp \
    recorderDialog.cpp \
    recorderThread.cpp \
    recorderControlWidget.cpp \                         # control widgets
    generatedParameters/recorder.cpp \                  # parameters for calculation threads, host dialogs etc.
    parametersMapper/parametersMapperRecorder.cpp \     # parameters for params mapper

FORMS += \
    ui/recorderControlWidget.ui \

#RESOURCES += ../../resources/main.qrc
