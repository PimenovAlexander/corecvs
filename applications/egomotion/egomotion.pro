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


TARGET   = egomotion
TEMPLATE = app

HOSTBASE_DIR=../base
include ($$HOSTBASE_DIR/baseApplication.pri)                   # it uses HOSTBASE_DIR, detects HOSTBASE_BINDIR, OBJECTS_DIR, ...

win32 {
    MOC_DIR = $$ROOT_DIR/.obj/$$TARGET_ORIG/$$BUILD_CFG_NAME  # resolve moc path for mocs to help qmake to unify those paths.
}

INCLUDEPATH += .



HEADERS += \
    \
    egomotionDialog.h \
    egomotionThread.h

SOURCES += \
    \
    egomotionDialog.cpp \
    main_egomotion.cpp \
    egomotionThread.cpp



# Generated stuff
INCLUDEPATH += generated
INCLUDEPATH += generated/ui


HEADERS += \
    parametersMapper/parametersMapperEgomotion.h \
    generated/egomotionParameters.h \
    generated/ui/egomotionParametersControlWidget.h \

SOURCES += \
    parametersMapper/parametersMapperEgomotion.cpp \
    generated/egomotionParameters.cpp \
    generated/ui/egomotionParametersControlWidget.cpp \

FORMS += \
    generated/ui/egomotionParametersControlWidget.ui \



OTHER_FILES +=                               \
    ../../tools/generator/xml/egomotion.xml  \
    ../../tools/generator/regen-egomotion.sh \

#RESOURCES += ../../resources/main.qrc
