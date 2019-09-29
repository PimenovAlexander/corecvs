# try use global config
exists(../../../../config.pri) {
    #message(Using global config)
    ROOT_DIR=../../../..
    include($$ROOT_DIR/config.pri)
} else {
    #message(Using local config)
    ROOT_DIR=../..
    include($$ROOT_DIR/cvs-config.pri)
}

DESTDIR = $$ROOT_DIR/bin
OBJECTS_DIR = $$ROOT_DIR/.obj/$$TARGET$$BUILD_CFG_NAME
MOC_DIR  = $$OBJECTS_DIR
UI_DIR   = $$OBJECTS_DIR
RCC_DIR  = $$OBJECTS_DIR

TEMPLATE = app
CONFIG  += console c++11
CONFIG  -= app_bundle
CONFIG  -= qt

include($$PWD/../../core/core.pri)

SOURCES += main.cpp

with_libjpeg {
    LIBJPEG_WRAPPER_DIR = ../../wrappers/libjpeg
    include($$LIBJPEG_WRAPPER_DIR/libjpeg.pri)
}

with_libpng {
    LIBPNG_WRAPPER_DIR = ../../wrappers/libpng
    include($$LIBPNG_WRAPPER_DIR/libpng.pri)
}

with_jsonmodern {
    message(We have Json for Modern C++ )
    JSONMODERN_WRAPPER_DIR = ../../wrappers/jsonmodern
    include($$JSONMODERN_WRAPPER_DIR/jsonmodern.pri)

    contains(DEFINES, WITH_JSONMODERN) {
        INCLUDEPATH += $$JSONMODERN_WRAPPER_DIR
    }
}

with_opencv {                                       # all this stuff was extracted from opencv.pri to speedup including
    OPENCV_WRAPPER_DIR = ../../wrappers/opencv
    include($$OPENCV_WRAPPER_DIR/opencv.pri)                # it uses OPENCV_WRAPPER_DIR inside

    contains(DEFINES, WITH_OPENCV) {
        INCLUDEPATH += $$OPENCV_WRAPPER_DIR
        INCLUDEPATH += $$OPENCV_WRAPPER_DIR/faceDetect
    }
}


OTHER_FILES +=

HEADERS +=
