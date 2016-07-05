# try use global config
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    ROOT_DIR=$$PWD/$$ROOT_DIR
    include($$ROOT_DIR/config.pri)
} else { 
    message(Using local config)
    ROOT_DIR=../..
    ROOT_DIR=$$PWD/$$ROOT_DIR
    include($$ROOT_DIR/cvs-config.pri)
}


QT += xml
TARGET   = robodetect
TEMPLATE = app

#OBJECTS_DIR = $$ROOT_DIR/.obj
#MOC_DIR = $$ROOT_DIR/.obj
#UI_DIR  = $$ROOT_DIR/.obj

# Using only open part of the project
UTILSDIR = $$PWD/../../utils
include($$UTILSDIR/utils.pri)

win32 {
    MOC_DIR = $${ROOT_DIR}/.obj/$$TARGET_ORIG/$$BUILD_CFG_NAME  # resolve moc path for mocs to help qmake to unify those paths.
}

HEADERS = \
    robodetectMainWindow.h \
    pointScene.h \
    localHistogram.h \
    widgets/robodetectImageWidget.h \
    gapFiller.h \
          

SOURCES = \
    robodetectMainWindow.cpp \
    main_robodetect.cpp \
    pointScene.cpp \
    localHistogram.cpp \
    widgets/robodetectImageWidget.cpp \

FORMS = ui/robodetectMainWindow.ui \

RESOURCES += ../../resources/main.qrc
RESOURCES += resources/robodetect.qrc
