# try use global config 
exists(../../../config.pri) {
    #message(Using global config)
    include(../../../config.pri)
} else { 
    message(Using local config)
    include(../cvs-config.pri)
}
 
TEMPLATE = subdirs

SUBDIRS +=     \
    grab24     \
    grab24_qt  \
    adoptcolor \
    fileloader \
    softrender  \
    raytracerender   \
#    stereo_generator \
    widgets_test \
    widget_harness \
#    command_harness \
    example_scene \
    flowtest \
    avencode \
    opencvpostcalib \
    focus_stack \
    pattern_detector \
    flow_detector \

!win32 {
    SUBDIRS += jitplayground
    SUBDIRS += gcodeplayground
}

with_qscript {
   SUBDIRS += qtScriptConsole
}

with_opencv {

    SUBDIRS +=       \
#   distortion_corrector \
#   matcher_basic \
#   matcher_full \
#   chessboard_detector \
    vodometry \
    stabilization \
    opencv_profile
}

grab24                              = grab/grab24.pro
grab24_qt                           = grab_qt/grab24_qt.pro

stabilization                       = stabilization/stabilization.pro
vodometry                           = vodometry/vodometry.pro
opencv_profile                      = opencv_profile/opencv_profile.pro

grab_N_captures                     = grab_N_captures/grab_N_captures.pro
serialize1                          = serialize1/serialize1.pro
jitplayground                       = jitplayground/jitplayground.pro
gcodeplayground                     = gcodeplayground/gcodeplayground.pro
example_scene                       = example_scene/example_scene.pro
adoptcolor                          = adoptcolor/adoptcolor.pro
avigrab                             = avigrab/avigrab.pro
decodebayer                         = decodebayer/decodebayer.pro
genvectorui                         = genvectorui/genvectorui.pro
aLowCodec                           = aLowCodec/aLowCodec.pro

cr2reader.file                      = cr2reader/cr2reader.pro
cr2reader.depends                  -= utils

debayer.file                        = debayer/debayer.pro
debayer.depends                    -= utils

softrender.file                     = softrender/softrender.pro
softrender.depends                 -= utils

raytracerender.file                 = raytracerender/raytracerender.pro
raytracerender.depends             -= utils

widgets_test.file                   = widgets_test/widgets_test.pro
widget_harness.file                 = widget_harness/widget_harness.pro
command_harness.file                = command_harness/command_harness.pro

new_board_detector	            = new_board_detector/new_board_detector.pro

distortion_corrector	            = distortion_corrector/distortion_corrector.pro

matcher_basic                       = matcher_basic/matcher_basic.pro
matcher_full                        = matcher_full/matcher_full.pro

chessboard_detector                 = chessboard_detector/chessboard_detector.pro
opencvpostcalib                     = opencvpostcalib/opencvpostcalib.pro

qtScriptConsole                     = qtScriptConsole/qtScriptConsole.pro

stereo_generator                    = stereo_generator/stereo_generator.pro

flowtest.file                       = flowtest/flowtest.pro

focusstacking.file                   = focusstacking/focusstacking.pro
pattern_detector.file                = pattern_detector/pattern_detector.pro
