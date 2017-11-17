# This file should be included by any project outside that wants to use core's files.
#
# input1 parameter: $$TARGET     - the current project output name
# output parameter: $$COREBINDIR - path to the output|used core library
#

!contains(CORECVS_INCLUDED, "core.pri") {
CORECVS_INCLUDED +=  core.pri

COREDIR=$$PWD

#
# Switching submodules on and off is not supported. However with some you can try. Risk is yours
#
CORE_SUBMODULES =       \
    alignment           \
    assignment          \
    automotive          \
    boosting            \
    buffers             \
    cammodel            \
    fileformats         \
    filesystem          \
    filters             \
    function            \
    geometry            \
    kalman              \
    kltflow             \
    math                \
    meta                \
    meanshift           \
    rectification       \
    reflection          \
    segmentation        \
    stats               \
    tbbwrapper          \
    utils               \
    clustering3d        \
#    features2d          \
    patterndetection    \
    cameracalibration   \
    device \
    graphs              \
    polynomial          \
    camerafixture       \
    iterative           \
    stereointerface     \

for (MODULE, CORE_SUBMODULES) {
    CORE_INCLUDEPATH += $${COREDIR}/$${MODULE}
}

# Some modules want to export more then one directory with includes. Add them here
CORE_INCLUDEPATH += \
    $$COREDIR/buffers/fixeddisp \
    $$COREDIR/buffers/flow \
    $$COREDIR/buffers/histogram \
    $$COREDIR/buffers/kernels \
    $$COREDIR/buffers/kernels/fastkernel \
    $$COREDIR/buffers/memory \
    $$COREDIR/buffers/morphological \
    $$COREDIR/buffers/rgb24 \
    $$COREDIR/filters/blocks \
    $$COREDIR/math/generic \
    $$COREDIR/math/matrix \
    $$COREDIR/math/sse \
    $$COREDIR/math/avx \
    $$COREDIR/math/vector \
    $$COREDIR/utils/visitors \
    $$COREDIR/clustering3d \
    $$COREDIR/geometry/raytrace \
    $$COREDIR/geometry/renderer \
    $$COREDIR/xml \
    $$COREDIR/xml/generated \
    $$COREDIR/tinyxml \
    $$COREDIR/../wrappers/cblasLapack \		# some of core's math algorithms use it

CORE_INCLUDEPATH_SUPP=$$COREDIR/..

!equals(SUPPRESSINCLUDES, "true") {
    INCLUDEPATH += $$CORE_INCLUDEPATH_SUPP $$CORE_INCLUDEPATH
} else {
    INCLUDEPATH += $$CORE_INCLUDEPATH_SUPP
    message(Per-Folder includes are supperssed. Only including $$CORE_INCLUDEPATH_SUPP)
}


DEPENDPATH  += $$CORE_INCLUDEPATH

exists($$COREDIR/../../../config.pri) {
    COREBINDIR = $$COREDIR/../../../bin
} else {
    message(Using local core. Global config should be at $$COREDIR/../../../config.pri)
    COREBINDIR = $$COREDIR/../bin
}

#message(Corebindir is $$COREBINDIR)

contains(TARGET, cvs_core): !contains(TARGET, cvs_core_restricted) {
    win32-msvc* {
        DEPENDPATH += $$COREDIR/xml                 # helps to able including sources by generated.pri from their dirs
    } else {
        DEPENDPATH += \
#           $$COREDIR \
            $$CORE_INCLUDEPATH                      # msvc sets this automatically by deps from includes for this project
    }
} else {
    !win32-msvc*: DEPENDPATH += $$CORE_INCLUDEPATH  # msvc sets this automatically by deps from includes for other projects! :(

    CORE_TARGET_NAME = cvs_core
    CORE_TARGET_NAME = $$join(CORE_TARGET_NAME,,,$$BUILD_CFG_SFX)

    LIBS = -L$$COREBINDIR -l$$CORE_TARGET_NAME $$LIBS  

    win32-msvc* {
        CORE_TARGET_NAME = $$join(CORE_TARGET_NAME,,,.lib)
    } else {
        CORE_TARGET_NAME = $$join(CORE_TARGET_NAME,,lib,.a)
    }
    PRE_TARGETDEPS += $$COREBINDIR/$$CORE_TARGET_NAME

    #message(Adding core lib to the linkinglist $$LIBS and targetdeps $$PRE_TARGETDEPS)
}

# The filesystem module needs this
with_unorthodox {
    DEFINES += CORE_UNSAFE_DEPS
}


!win32  {
    LIBS += -lstdc++fs
}


} # !contains(CORECVS_INCLUDED, "core.pri")


