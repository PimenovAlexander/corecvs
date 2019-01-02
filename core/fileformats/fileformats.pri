HEADERS += \
    $$PWD/bufferLoader.h \
    $$PWD/bmpLoader.h \
    $$PWD/ppmLoader.h \
    $$PWD/rawLoader.h \
    $$PWD/plyLoader.h \
    $$PWD/stlLoader.h \
    $$PWD/metamap.h \
    $$PWD/svgLoader.h \
    $$PWD/floLoader.h \
    $$PWD/openCVDataLoader.h \
    $$PWD/tgaLoader.h

SOURCES += \
    $$PWD/bufferLoader.cpp \
    $$PWD/bmpLoader.cpp \
    $$PWD/ppmLoader.cpp \
    $$PWD/rawLoader.cpp \
    $$PWD/plyLoader.cpp \
    $$PWD/stlLoader.cpp \
    $$PWD/svgLoader.cpp \
    $$PWD/floLoader.cpp \
    $$PWD/openCVDataLoader.cpp \
    $$PWD/tgaLoader.cpp

CONFIG += with_formats3d

with_formats3d {

HEADERS += \
    fileformats/meshLoader.h \
    fileformats/objLoader.h \
    fileformats/gcodeLoader.h \
    fileformats/pltLoader.h \
    fileformats/xyzListLoader.h \


SOURCES += \
    fileformats/meshLoader.cpp \
    fileformats/objLoader.cpp \
    fileformats/gcodeLoader.cpp \
    fileformats/pltLoader.cpp \
    fileformats/xyzListLoader.cpp \

}
