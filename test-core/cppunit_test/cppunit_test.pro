TARGET = cppunit

include(../test.pri)


SOURCES += \
        cppunit_test.cpp \
        MatcherTest.cpp

HEADERS += MatcherTest.h

LIBS += -L/usr/lib/ -lcppunit
