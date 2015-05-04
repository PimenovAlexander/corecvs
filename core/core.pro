# try use global config 
exists(../../../config.pri) {
    #message(Using global config)
    ROOT_DIR=../../../
} else { 
    message(Using local config)
    ROOT_DIR=../    
}
include($$ROOT_DIR/config.pri)
    

CONFIG  += staticlib
TARGET   = cvs_core
TEMPLATE = lib

COREDIR = .
include(core.pri)                                   # it uses COREDIR, TARGET and detects COREBINDIR!

OBJECTS_DIR = $$ROOT_DIR/.obj/cvs_core$$BUILD_CFG_NAME
MOC_DIR     = $$OBJECTS_DIR
UI_DIR      = $$OBJECTS_DIR

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

include(xml/generated/generated.pri)
