#-------------------------------------------------
#
# Project created by QtCreator 2012-12-03T16:45:00
#
#-------------------------------------------------

STATEMACHINEPATH="../../qt4/host-soft/stateMachine"

include(../testsCommon.pri)

STATEMACHINEPATH = "../../qt4/host-soft/stateMachine"

INCLUDEPATH += "../../qt4/host-soft/stateMachine" \
                "../../qt4/host-soft" \
                "../../qt4/host-base"

include(../../qt4/host-soft/stateMachine/stateMachine.pri)

SOURCES += main.cpp \
    test.cpp

HEADERS += \
    test.h
