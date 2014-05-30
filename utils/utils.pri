# This file must be included for any external usage of cvs_utils library!
#
# Input1 parameter  - $$UTILSDIR
# Input2 parameter  - $$TARGET
# Input3 parameter  - $$ROOT_DIR
# Output parameters - $$UTILS_BINDIR
#

COREDIR=../core
include($$COREDIR/core.pri)                         # it uses COREDIR, TARGET and detects     COREBINDIR!

UTILSDIR=$$PWD

UTILS_INCLUDEPATH = \
    $$UTILSDIR \
    $$UTILSDIR/3d \
    $$UTILSDIR/camcalc \
    $$UTILSDIR/corestructs \
    $$UTILSDIR/corestructs/coreWidgets \
    $$UTILSDIR/corestructs/libWidgets \
    $$UTILSDIR/corestructs/parametersMapper \
    $$UTILSDIR/distortioncorrector \                # include isn't used, but need for DEPENDPATH!
    $$UTILSDIR/fileformats \
    $$UTILSDIR/filters \
    $$UTILSDIR/filters/graph \
    $$UTILSDIR/filters/ui \                         # include isn't used, but need for DEPENDPATH!
#   $$UTILSDIR/flowcolorers \
    $$UTILSDIR/framesources \
    $$UTILSDIR/framesources/directShow \    
    $$UTILSDIR/framesources/decoders \    
    $$UTILSDIR/framesources/v4l2 \
    $$UTILSDIR/processor \
    $$UTILSDIR/rectifier \
#   $$UTILSDIR/serializer \     # obsolete?
    $$UTILSDIR/statistics \     # obsolete?
    $$UTILSDIR/uis \
    $$UTILSDIR/uis/cloudview \    
    $$UTILSDIR/visitors \
    $$UTILSDIR/widgets \

INCLUDEPATH += $$UTILS_INCLUDEPATH

UTILS_BINDIR = $$ROOT_DIR/bin
#message(Utils.pri ROOT_DIR is <$$ROOT_DIR>. Bindir is <$$UTILS_BINDIR>. PWD is <$$PWD>)

contains(TARGET, cvs_utils) {
    !win32-msvc*: DEPENDPATH += $$UTILS_INCLUDEPATH # it should set automatically by dependencies from includes!
                                                    # Nevertheless mingw compiler requires this when utils's ui-sources use utils's ui-sources.
} else {
    !win32-msvc*: DEPENDPATH += $$UTILS_INCLUDEPATH # msvc sets this automatically by deps from includes for other projects! :(

    # The "UTILS_INTDIR" path should be known for other projects that use "cvs_utils"
    #
    UTILS_INTDIR = $$ROOT_DIR/.obj/cvs_utils$$BUILD_CFG_NAME

    # this is needed as some new sources of cvs_utils use ui-sources from this project!
    INCLUDEPATH += $$UTILS_INTDIR                       # add auto-generated ui* headers of cvs_utils into include path for the current project

    UTILS_TARGET_NAME = cvs_utils
    UTILS_TARGET_NAME = $$join(UTILS_TARGET_NAME,,,$$BUILD_CFG_SFX)

    LIBS = -L$$UTILS_BINDIR -l$$UTILS_TARGET_NAME $$LIBS

    win32-msvc* {
        UTILS_TARGET_NAME = $$join(UTILS_TARGET_NAME,,,.lib)
    } else {
        UTILS_TARGET_NAME = $$join(UTILS_TARGET_NAME,,lib,.a)
    }
    PRE_TARGETDEPS += $$UTILS_BINDIR/$$UTILS_TARGET_NAME
}

DESTDIR = $$UTILS_BINDIR


#CONFIG += with_opengl                           # always include here OpenGL dependent modules as utils's and related projects need it
with_opengl {
    QT += opengl                                # this must be defined for utils's and all related sources

   #unix:LIBS += -lGL -lQtOpenGL                # Qt must add this

    win32 {
        LIBS += -lglu32 -lopengl32              # these libs must be exactly here: before openCV but after our libs! It's a magic of mingw, for msvc it's easier.:)
    } else {
        LIBS += -lXtst -lX11 -lXext -lGLU       # these libs must be exactly here: they're required by OpenGL and some other stuff...
    }

    INCLUDEPATH += $$UTILSDIR/opengl
    
    with_openglext {
        DEFINES += WITH_OPENGLEXT
    } 

}

with_ueye {
    UEYE_PATH = $$(UEYE_PATH)
    win32 {
        isEmpty(UEYE_PATH): UEYE_PATH = "C:/Program Files/IDS/uEye"

        # Try to use provided install path
        exists($$UEYE_PATH/Develop/lib/uEye_api_64.lib) {
            DEFINES     += WITH_UEYE
            INCLUDEPATH +=   $$UEYE_PATH/Develop/include
            LIBS        += -L$$UEYE_PATH/Develop/lib -luEye_api_64

            !build_pass:message(Using uEye <$$UEYE_PATH>)
        } else {
            !build_pass:message(Unable to find uEye at <$$UEYE_PATH>)
        }
    } else:exists("/usr/lib/libueye_api.so") {
        DEFINES += WITH_UEYE
        LIBS    += -lueye_api
    } else {
        !build_pass:message(Unable to find uEye at "/usr/lib/libueye_api.so")
    }
}

with_opencv {                                       # all this stuff was extracted from opencv.pri to speedup including
    OPENCV_WRAPPER_DIR = $$UTILSDIR/../wrappers/opencv
    include($$OPENCV_WRAPPER_DIR/opencvLibs.pri)

    contains(DEFINES, WITH_OPENCV) {
        INCLUDEPATH += $$OPENCV_WRAPPER_DIR
        INCLUDEPATH += $$OPENCV_WRAPPER_DIR/faceDetect
    }    
}

with_directshow {
    DIRECT_SHOW_WRAPPER_DIR = $$UTILSDIR/../wrappers/directShow
    include($$DIRECT_SHOW_WRAPPER_DIR/directShowLibs.pri)
}


###############################################
#   Useful common part for all cvs projects   #
###############################################

isEmpty(TARGET_ORIG) {                              # be careful of multiple including of this file
    TARGET_ORIG = $$TARGET
    TARGET      = $$join(TARGET,,,$$BUILD_CFG_SFX)  # add 'd' at the end for debug versions
} else {
   #message(TARGET_ORIG=$$TARGET_ORIG is not modified)
}

OBJECTS_DIR = $$ROOT_DIR/.obj/$$TARGET_ORIG$$BUILD_CFG_NAME

MOC_DIR = $$OBJECTS_DIR                             # not "$$ROOT_DIR/.obj/$$TARGET_ORIG" as the compiler regenerates them if config has been changed
UI_DIR  = $$OBJECTS_DIR
RCC_DIR = $$OBJECTS_DIR

DESTDIR = $$UTILS_BINDIR

# to delete also target lib by 'clean' make command (distclean does this)
win32 {
    QMAKE_CLEAN += "$(DESTDIR_TARGET)"              # for Linux qmake doesn't generate DESTDIR_TARGET :(
} else {
    QMAKE_CLEAN += "$(DESTDIR)$(TARGET)"            # for win such cmd exists with inserted space :(
}

# QMake's bug found on mingw-win config:
# 1. hostBase.pri is included here from current dir and it includes utils.pri from its dir.
#    So the path to mocinclude.tmp for derived projects (host-*) is created as:
#    "../utils/../../../.obj/$$TARGET_ORIG" that is the same as "../../../.obj/$$TARGET_ORIG".
# 2. It generates dependency to this file without PWD (from curDir) for all mocs,
#    but the rule for mocinclude.tmp uses PWD for it.
# So they're unequal from the generated makefile view point!
#
# But indeed it's fixed by using ROOT_DIR for all projects!
#
win32 {
    # MOC_DIR = $$ROOT_DIR/.obj/$$TARGET_ORIG$$BUILD_CFG_NAME  # resolve moc path for mocs to help qmake to unify those paths.
    # message(TARGET_ORIG=$$TARGET_ORIG  MOC_DIR=$$MOC_DIR)

    QMAKE_CLEAN += "$$MOC_DIR/mocinclude.tmp"       # it doesn't killed some-why...
}
