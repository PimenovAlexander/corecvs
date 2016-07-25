# This file should be included by any project outside that wants to use core's files.
#
# input1 parameter: $$TARGET     - the current project output name
# output parameter: $$COREBINDIR - path to the output|used core library
#

COREDIR=$$PWD

#
# Switching submodules on and off it not supported. However with some you can try. Risk is yours
#
CORE_SUBMODULES= \
    alignment     \
    assignment    \
    automotive    \
    boosting      \
    buffers       \
    cammodel      \
    fileformats   \
    filesystem    \
    filters       \
    function      \
    geometry      \
    kalman        \
    kltflow       \
    math          \
    meta          \
    meanshift     \
    rectification \
    reflection    \
    segmentation  \
    stats         \
    tbbwrapper    \
    utils         \
    clustering3d  \
    features2d    \
    patterndetection \
    cameracalibration \
   # graphs        \
    polynomial    \
    camerafixture \

with_blas {
   # CORE_SUBMODULES += reconstruction
}

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


INCLUDEPATH += $$CORE_INCLUDEPATH
DEPENDPATH  += $$CORE_INCLUDEPATH

exists(../../../config.pri) {
    COREBINDIR = $$COREDIR/../../../bin
} else {
    message(Using local core. Global config should be at $$COREDIR/../../../config.pri)
    COREBINDIR = $$COREDIR/../bin
}

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
}

# The filesystem module needs this
with_unorthodox {
!win32  {
    LIBS += -lstdc++fs
}
    DEFINES += CORE_UNSAFE_DEPS
}

