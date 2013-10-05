# try use global config 
exists(../../../config.pri) {
    #message(Using global config)
    TOPLEVEL=../../../
} else { 
    message(Using local config)
    TOPLEVEL=../    
}
 include($$TOPLEVEL/config.pri)
    

CONFIG  += staticlib
TARGET   = cvs_core
TEMPLATE = lib

COREDIR = .
include(core.pri)               # it uses COREDIR, TARGET and detects COREBINDIR!

CORE_INTDIR = $$TOPLEVEL/.obj/core
win32 {
    OBJECTS_DIR = $$CORE_INTDIR/$$BUILD_CFG_NAME
} else {
    OBJECTS_DIR = $$CORE_INTDIR
}
MOC_DIR = $$CORE_INTDIR
UI_DIR  = $$CORE_INTDIR

# Note: debug and release libs will be overwritten on !win32 only
#
TARGET  = $$join(TARGET,,,$$BUILD_CFG_SFX)

DESTDIR = $$COREBINDIR

# to delete also target lib by 'clean' make command (distclean does this)
win32 {
    QMAKE_CLEAN += "$(DESTDIR_TARGET)"          # for linux qmake doesn't generate DESTDIR_TARGET :(
} else {
    QMAKE_CLEAN += "$(DESTDIR)$(TARGET)"        # for win such cmd exists with inserted space :(
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
include(xml/generated/generated.pri)
include(clustering3d/clustering3d.pri)
