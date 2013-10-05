include(../../config.pri)

QT     -= gui
CONFIG += console

TARGET  = test_directshow

DSHOW_INTDIR = ../../../../.obj/tests/directshow
win32 {
    OBJECTS_DIR = $$DSHOW_INTDIR/$$BUILD_CFG_NAME
} else {
    OBJECTS_DIR = $$DSHOW_INTDIR
}
MOC_DIR = $$OBJECTS_DIR			            # we have to set this to omit creating dummy dirs: debug,release

# Note: debug and release libs will be overwritten on !win32 only
#
TARGET  = $$join(TARGET,,,$$BUILD_CFG_SFX)          # add 'd' at the end for debug win32 version

DESTDIR = ../../../../bin

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
