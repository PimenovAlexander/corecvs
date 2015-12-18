# try use global config 
exists(../../../config.pri) {
    #message(Using global config)
    ROOT_DIR=../../..
    include($$ROOT_DIR/config.pri)
} else { 
    message(Using local config)
    ROOT_DIR=..
    include($$ROOT_DIR/cvs-config.pri)
}

TEMPLATE = lib
TARGET   = cvs_core
CONFIG  += staticlib

include(core.pri)                                   # it uses TARGET and detects COREBINDIR!
include($$ROOT_DIR/git-version.pri)

OBJECTS_DIR = $$ROOT_DIR/.obj/cvs_core$$BUILD_CFG_NAME
MOC_DIR     = $$OBJECTS_DIR
UI_DIR      = $$OBJECTS_DIR
RCC_DIR     = $$OBJECTS_DIR

TARGET  = $$join(TARGET,,,$$BUILD_CFG_SFX)          # add 'd' at the end for debug versions

DESTDIR = $$COREBINDIR

# to delete also target lib by 'clean' make command (distclean does this)
win32 {
    QMAKE_CLEAN += "$(DESTDIR_TARGET)"              # for Linux qmake doesn't generate DESTDIR_TARGET :(
} else {
    QMAKE_CLEAN += "$(DESTDIR)$(TARGET)"            # for win such cmd exists with inserted space :(
}

#
# include sources and headers for each subdir
#
include(alignment/alignment.pri)
include(assignment/assignment.pri)
include(automotive/automotive.pri)
include(boosting/boosting.pri)
include(buffers/buffers.pri)
include(cammodel/cammodel.pri)
include(fileformats/fileformats.pri)
include(filters/filters.pri)
include(function/function.pri)
include(geometry/geometry.pri)
include(kalman/kalman.pri)
include(kltflow/kltflow.pri)
include(math/math.pri)
include(meanshift/meanshift.pri)
include(rectification/rectification.pri)
include(reflection/reflection.pri)
include(segmentation/segmentation.pri)
include(stats/stats.pri)
include(tbbwrapper/tbbwrapper.pri)
include(utils/utils.pri)
include(clustering3d/clustering3d.pri)
#include(features2d/features2d.pri)
#include(patterndetection/patterndetection.pri)
#include(cameracalibration/cameracalibration.pri)
#include(graphs/graphs.pri)
#include(reconstruction/reconstruction.pri)
#include(polynomial/polynomial.pri)
include(meta/meta.pri)


include(xml/generated/generated.pri)



OTHER_FILES +=            \
    xml/parameters.xml    \
    xml/bufferFilters.xml \
    xml/clustering1.xml   \
    xml/filterBlock.xml   \
    xml/precise.xml       \
    xml/distortion.xml    \

OTHER_FILES +=            \
    ../tools/generator/regen-core.sh \
    ../tools/generator/h_stub.sh \


