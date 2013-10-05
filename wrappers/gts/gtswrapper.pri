#TARGET = cvs_core
#
#TEMPLATE = lib
#
#OBJECTS_DIR = .obj
#MOC_DIR = .moc
#UI_DIR = .ui

LIBS += $$system(pkg-config --libs gts)

QMAKE_CXXFLAGS += $$system(pkg-config --cflags gts)

HEADERS += \    
    $$GTSDIR/gtswrapper.h \
    

SOURCES += \
    $$GTSDIR/gtswrapper.cpp \

INCLUDEPATH += /usr/include/glib-2.0/ /usr/lib/glib-2.0/include/
