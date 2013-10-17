# This file should be included by any project outside that wants to use core's files.
#
# input1 parameter: $$COREDIR    - path to core project files
# input2 parameter: $$TARGET     - the current project output name

# output parameter: $$COREBINDIR - path to the output|used core library
#

COREDIR=$$PWD

CORE_INCLUDEPATH = \
    $$COREDIR/alignment \
#   $$COREDIR/alignment/camerasCalibration \    # used via path
    $$COREDIR/assignment \
    $$COREDIR/automotive \
#   $$COREDIR/automotive/simulation \           # not used, obsolete
    $$COREDIR/boosting \
    $$COREDIR/buffers \
#   $$COREDIR/buffers/converters \              # not used
    $$COREDIR/buffers/fixeddisp \
    $$COREDIR/buffers/flow \
    $$COREDIR/buffers/histogram \
    $$COREDIR/buffers/kernels \
#   $$COREDIR/buffers/kernels/fastconverter \   # not used ?
    $$COREDIR/buffers/kernels/fastkernel \
    $$COREDIR/buffers/memory \
    $$COREDIR/buffers/morphological \
    $$COREDIR/buffers/rgb24 \
#   $$COREDIR/buffers/voxels \                  # not used
    $$COREDIR/cammodel \
#   $$COREDIR/clegacy \                         # not used ?
#   $$COREDIR/clegacy/math \                    # not used
    $$COREDIR/fileformats \
    $$COREDIR/filters \
    $$COREDIR/filters/blocks \
    $$COREDIR/function \
    $$COREDIR/geometry \
    $$COREDIR/kalman \
    $$COREDIR/kltflow \
    $$COREDIR/math \
#   $$COREDIR/math/avx \                        # not used
#   $$COREDIR/math/fixed \                      # not used
    $$COREDIR/math/generic \
    $$COREDIR/math/matrix \
#   $$COREDIR/math/neon \
    $$COREDIR/math/sse \
    $$COREDIR/math/vector \
    $$COREDIR/meanshift \
    $$COREDIR/rectification \
    $$COREDIR/reflection \
    $$COREDIR/segmentation \
#   $$COREDIR/serializer \                      # not used
    $$COREDIR/stats \
    $$COREDIR/tbbwrapper \
    $$COREDIR/tinyxml \
    $$COREDIR/utils \
    $$COREDIR/utils/visitors \
    $$COREDIR/clustering3d \
    $$COREDIR/xml \
    $$COREDIR/xml/generated \                   # to allow including of generated headers without directory name prefix

INCLUDEPATH += $$CORE_INCLUDEPATH

exists(../../../config.pri) {
    COREBINDIR = $$COREDIR/../../../bin
} else {
    message(Using local core. config should be $$COREDIR/../../../config.pri)
    COREBINDIR = $$COREDIR/../bin
}


contains(TARGET, cvs_core): !contains(TARGET, cvs_core_restricted) {
    win32-msvc* {
        DEPENDPATH += $$COREDIR/xml             # helps to able including sources by generated.pri from their dirs
    }
    else {
        DEPENDPATH += \
#           $$COREDIR \
            $$CORE_INCLUDEPATH                      # msvc sets this automatically by deps from includes for this project
    }
} else {
    !win32-msvc*: DEPENDPATH += $$CORE_INCLUDEPATH  # msvc sets this automatically by deps from includes for other projects! :(

    CORE_TARGET_NAME = cvs_core
    CORE_TARGET_NAME = $$join(CORE_TARGET_NAME,,,$$BUILD_CFG_SFX)

    LIBS = \
        -L$$COREBINDIR \
        -l$$CORE_TARGET_NAME \
        $$LIBS

    win32-msvc* {
        CORE_TARGET_NAME = $$join(CORE_TARGET_NAME,,,.lib)
        PRE_TARGETDEPS  += $$COREBINDIR/$$CORE_TARGET_NAME
    } else {
        CORE_TARGET_NAME = $$join(CORE_TARGET_NAME,,lib,.a)
        PRE_TARGETDEPS  += $$COREBINDIR/$$CORE_TARGET_NAME
    }
}
