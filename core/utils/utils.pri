HEADERS += \
    utils/global.h \
    utils/stdint_win.h \
    utils/preciseTimer.h \
    utils/propertyList.h \
    utils/visitors/propertyListVisitor.h \
    utils/utils.h \
    utils/visitors/basePathVisitor.h \
    utils/log.h \
    utils/countedPtr.h \
    utils/atomicOps.h \
    utils/typesafeBitmaskEnums.h \
    utils/statusTracker.h \
#    utils/abstractImageNamer.h \
    utils/statusTrackerCatcher.h \
    $$PWD/debuggableBlock.h

SOURCES += \
    utils/memhooks.c \
    utils/util.c \
    utils/preciseTimer.cpp \
    utils/propertyList.cpp \
    utils/visitors/propertyListVisitor.cpp \
    utils/visitors/basePathVisitor.cpp \
    utils/utils.cpp \
    utils/log.cpp \
    utils/statusTracker.cpp \
#    utils/abstractImageNamer.cpp \
    $$PWD/debuggableBlock.cpp

