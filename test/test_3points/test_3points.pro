ROOT_DIR=../../../../..
ROOT_DIR=$$PWD/$$ROOT_DIR
include($$ROOT_DIR/config.pri)

TEMPLATE = app
TARGET   = topcon_test_3pointtest
CONFIG  += console

#DESTDIR = $$ROOT_DIR/bin

include($$ROOT_DIR/src/open/wrappers/gtest/gtest.pri)

include($$ROOT_DIR/src/open/utils/utils.pri)

include($$ROOT_DIR/src/restricted/topcon/utils/topcon_utils_restricted.pri)

SOURCES += *.cpp
HEADERS += *.h

OTHER_FILES += *.sh
OTHER_FILES += *.xml

INCLUDEPATH += gen
SOURCES += gen/*.cpp
HEADERS += gen/*.h
