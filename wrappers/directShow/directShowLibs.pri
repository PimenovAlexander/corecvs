
!contains(QMAKE_HOST.arch, x86_64) {
    #!build_pass:message("Using capdll 32 bits. x86 build")
    CAPDLL_LIB_DIR=lib
} else {
    #!build_pass:message("Using capdll 64 bits. x86_64 build")
    CAPDLL_LIB_DIR=lib64
}

DSHOW_LIB_PATH = $$DIRECT_SHOW_WRAPPER_DIR/$$CAPDLL_LIB_DIR

!build_pass: contains(TARGET, cvs_utils): message("Using capdll at <$$DSHOW_LIB_PATH>")

HEADERS     +=   $$DSHOW_LIB_PATH/capdll.h
DEPENDPATH  +=   $$DSHOW_LIB_PATH
INCLUDEPATH +=   $$DSHOW_LIB_PATH

win32-msvc* {
           win32-msvc2010 {	CAPDLL_MSVC_VER = capdll_vc10
    } else:win32-msvc2013 {	CAPDLL_MSVC_VER = capdll_vc12
    } else:win32-msvc2015 {	CAPDLL_MSVC_VER = capdll_vc14
    } else {                CAPDLL_MSVC_VER = capdll_vc1x
    }
    LIBS  += -L$$DSHOW_LIB_PATH -l$$CAPDLL_MSVC_VER
} else {
    # The "mingw" toolchain is able to generate library file from the present capdll.dll module at the same dir.
    # The capdll.lib presence from MSVC at that dir disturbs it!
    #
    LIBS    += -L$$DSHOW_LIB_PATH -lcapdll
}
