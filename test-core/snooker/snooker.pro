##################################################################
# snooker.pro created on Apr 08, 2011
# This is a file for QMAKE that allows to build the test snooker
#
##################################################################
include(../testsCommon.pri)

HEADERS += \
   errors.h \
   commonTypes.h \
   snookerSegmentator.h \
   reflectionSegmentator.h \


SOURCES += \
   main_test_snooker.cpp \
   reflectionSegmentator.cpp \
   snookerSegmentator.cpp \
   detector.cpp \
