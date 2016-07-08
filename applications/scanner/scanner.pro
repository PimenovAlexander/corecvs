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


TARGET   = scanner
TEMPLATE = app

HOSTBASE_DIR=../base
include ($$HOSTBASE_DIR/baseApplication.pri)                   # it uses HOSTBASE_DIR, detects HOSTBASE_BINDIR, OBJECTS_DIR, ...

win32 {
    MOC_DIR = $$ROOT_DIR/.obj/$$TARGET_ORIG/$$BUILD_CFG_NAME  # resolve moc path for mocs to help qmake to unify those paths.
}

INCLUDEPATH += .

HEADERS += \
    scannerDialog.h \
    scannerThread.h \
    scannerControlWidget.h \                           # control widgets
    generatedParameters/scanner.h \                    # parameters for calculation threads, host dialogs etc.
    parametersMapper/parametersMapperScanner.h \       # parameters for params mapper

SOURCES += \
    main_scanner.cpp \
    scannerDialog.cpp \
    scannerThread.cpp \
    scannerControlWidget.cpp \                         # control widgets
    generatedParameters/scanner.cpp \                  # parameters for calculation threads, host dialogs etc.
    parametersMapper/parametersMapperScanner.cpp \     # parameters for params mapper

FORMS += \
    ui/scannerControlWidget.ui \

#RESOURCES += ../../resources/main.qrc
