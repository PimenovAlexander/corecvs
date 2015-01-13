# try use global config
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
} else { 
    message(Using local config)
    ROOT_DIR=../..
}
!win32 {                                        # it dues to the "mocinclude.tmp" bug on win32!
    ROOT_DIR=$$PWD/$$ROOT_DIR
}
include($$ROOT_DIR/config.pri)

QT += xml
TARGET   = testbed
TEMPLATE = app

# Using only open part of the project
UTILSDIR = ../../utils
include($$UTILSDIR/utils.pri)


HEADERS = \
    testbedMainWindow.h \
#    pointScene.h \

          

SOURCES = \
    testbedMainWindow.cpp \
    main_testbed.cpp \
#    pointScene.cpp \

FORMS = ui/testbedMainWindow.ui \

#RESOURCES += ../../resources/main.qrc
