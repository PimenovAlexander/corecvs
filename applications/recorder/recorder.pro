# try use global config 
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    #message(Using global config)
} else { 
    message(Using local config)
    ROOT_DIR=../..
}
ROOT_DIR=$$PWD/$$ROOT_DIR
include($$ROOT_DIR/config.pri)


TARGET   = recorder
TEMPLATE = app

HOSTBASE_DIR=../base
include ($$HOSTBASE_DIR/baseApplication.pri)                   # it uses HOSTBASE_DIR, detects HOSTBASE_BINDIR, OBJECTS_DIR, ...

win32 {
    MOC_DIR = $$ROOT_DIR/.obj/$$TARGET_ORIG/$$BUILD_CFG_NAME  # resolve moc path for mocs to help qmake to unify those paths.
}

INCLUDEPATH += .

HEADERS += \
    recorderDialog.h \
    recorderThread.h \
    recorderControlWidget.h \                           # control widgets
    generatedParameters/recorder.h \                    # parameters for calculation threads, host dialogs etc.
    parametersMapper/parametersMapperRecorder.h \       # parameters for params mapper

SOURCES += \
    main_virecorder.cpp \
    recorderDialog.cpp \
    recorderThread.cpp \
    recorderControlWidget.cpp \                         # control widgets
    generatedParameters/recorder.cpp \                  # parameters for calculation threads, host dialogs etc.
    parametersMapper/parametersMapperRecorder.cpp \     # parameters for params mapper

FORMS += \
    ui/recorderControlWidget.ui \

