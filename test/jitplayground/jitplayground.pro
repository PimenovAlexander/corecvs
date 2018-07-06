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
TARGET   = test_jitplayground
QT 	+= xml

include(../../utils/utils.pri)                        # it uses TARGET, ROOT_DIR and detects UTILS_BINDIR, OBJECTS_DIR, DESTDIR, ...!

SOURCES += main_jitplayground.cpp \
    reprojectionCostFunction.cpp \
    dllFunction.cpp \


HEADERS += main_jitplayground.h \
    reprojectionCostFunction.h \
    dllFunction.h \

!win32 {
  LIBS += -ldl
}
