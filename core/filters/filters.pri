
HEADERS +=   \
    filters/newstyle/*.h \

SOURCES +=   \
    filters/newstyle/*.cpp \


OLDFILTERS_HEADERS +=   \
    filters/legacy/*.h \
    filters/legacy/blocks/*.h \

OLDFILTERS_SOURCES +=   \
    filters/legacy/*.cpp \
    filters/legacy/blocks/*.cpp \


#with_oldfilters {
    HEADERS += $${OLDFILTERS_HEADERS}
    SOURCES += $${OLDFILTERS_SOURCES}
#} else {
#    OTHER_FILES += $${OLDFILTERS_HEADERS} $${OLDFILTERS_SOURCES}
#}
OTHER_FILES +=$$PWD/CMakeLists.txt
