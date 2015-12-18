# try use global config 
exists(../../../../config.pri) {
    ROOT_DIR=../../../..
    #message(Using global config)
    include($$ROOT_DIR/config.pri)
} else { 
    message(Using local config)
    ROOT_DIR=../../
    include($$ROOT_DIR/cvs-config.pri)
}
ROOT_DIR=$$PWD/$$ROOT_DIR

TEMPLATE = app
TARGET   = test_directshow
CONFIG  += console
QT      -= gui


OBJECTS_DIR = $$ROOT_DIR/.obj/directshow$$BUILD_CFG_NAME

MOC_DIR = $$OBJECTS_DIR			                    # we have to set this to omit creating dummy dirs: debug,release

TARGET  = $$join(TARGET,,,$$BUILD_CFG_SFX)          # add 'd' at the end for debug versions

DESTDIR = $$ROOT_DIR/bin

# We should copy proper version of capdll.dll module from LIB dir of this project into the output common BIN dir
#
win32-msvc*:!contains(QMAKE_HOST.arch, x86_64) {
        QMAKE_POST_LINK = copy /Y .\\lib\\capdll.dll   ..\\..\\..\\..\\bin
} else:win32-msvc* {
        QMAKE_POST_LINK = copy /Y .\\lib64\\capdll.dll ..\\..\\..\\..\\bin
} else:win32 {
	SHELL=$$(SHELL)
	isEmpty(SHELL) {
		QMAKE_POST_LINK = copy /Y .\\lib64\\capdll.dll ..\\..\\..\\..\\bin
	} else {
		message(Postbuild directshow: copy is not supported in mingw-cmd-mingw32-make call chain)
	}
} else {
        QMAKE_POST_LINK = "echo Nothing to do on Unix with capdll!"
}

DIRECT_SHOW_WRAPPER_DIR = .
include($$DIRECT_SHOW_WRAPPER_DIR/directShowLibs.pri)

SOURCES += $$DIRECT_SHOW_WRAPPER_DIR/test.cpp
