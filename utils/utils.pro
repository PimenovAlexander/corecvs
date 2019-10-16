# try use global config
exists(../../../config.pri) {
    ROOT_DIR=../../..
    #message(Using global config)
    include($$ROOT_DIR/config.pri)
} else {
    message(Using local config)
    ROOT_DIR=..
    include($$ROOT_DIR/cvs-config.pri)
}
ROOT_DIR=$$PWD/$$ROOT_DIR

TEMPLATE = lib
TARGET   = cvs_utils
CONFIG  += staticlib

include($$ROOT_DIR/git-version.pri)

include(utils.pri)                      # it uses TARGET and detects UTILS_BINDIR, OBJECTS_DIR,...!

QT += gui

# TODO: split this huge project into parts by these keys: utils, utils_gui?
CONFIG += with_filters
CONFIG += with_widgets

#
# In global scope we have non UI classes
#
HEADERS += \
#    frames.h \
#    framesources/imageCaptureInterface.h \
#    framesources/cameraControlParameters.h \
#    framesources/decoders/mjpegDecoder.h \
#    framesources/decoders/mjpegDecoderLazy.h \
#    framesources/decoders/decoupleYUYV.h \
#    framesources/decoders/aLowCodec.h \
    \
#   framesources/file/core/framesources/file/imageFileCaptureInterface.h \
#   framesources/file/abstractFileCapture.h \
#   framesources/file/abstractFileCaptureSpinThread.h \
    \
    fileformats/qtFileLoader.h \
    \
    flowDrawer.h \
    flowcolorers/abstractFlowColorer.h \
    flowcolorers/flowColorer.h \
    flowcolorers/stereoColorer.h \
    \
    qtHelper.h \
    timeliner.h \
    \
    visitors/baseXMLVisitor.h \
    visitors/xmlGetter.h \
    visitors/xmlSetter.h \
    visitors/qSettingsSetter.h \
    visitors/qSettingsGetter.h \
    \
    serializer/serializer.h \
    serializer/serializedWidget.h \
    serializer/widgetQtIterator.h \
    \
    statistics/contentStatistics.h \
    statistics/userPoll.h \
    \
    rectifier/estimationMethodType.h \
    rectifier/optimizationMethodType.h \
    rectifier/universalRectifier.h \
    rectifier/rectifyParameters.h \
    \
##  opengl/openGLTools.h \
    \
    distortioncorrector/distortionParameters.h \
    camcalc/colorTimer.h \
    \
#    Generated    \
    \
    widgets/generated/graphPlotParameters.h \
    widgets/generated/graphStyle.h \
    \
    configManager.h \
    \
    corestructs/lockableObject.h \
    corestructs/g12Image.h \
    \
    visitors/jsonGetter.h \
    visitors/jsonSetter.h \
    widgets/vectorWidget.h \
    distortioncorrector/cameraModelParametersControlWidget.h \
    distortioncorrector/lensDistortionModelParametersControlWidget.h \
    distortioncorrector/calibrationFeaturesWidget.h \
    uis/cloudview/scene3dTreeView.h \
    scannercontrol.h \
#    capture/abstractManipulatorInterface.h \
    corestructs/cameraModel/affine3dControlWidget.h \
    corestructs/cameraModel/fixtureControlWidget.h \
    widgets/observationListModel.h \
    distortioncorrector/pointListEditImageWidget.h \
    corestructs/cameraModel/featurePointControlWidget.h \
    uis/aboutPropsTableWidget.h \
    uis/histogramDepthDialog.h \
    corestructs/reflectionWidget.h \
    scripting/coreToScript.h \
    corestructs/cameraModel/fixtureGeometryControlWidget.h \
    corestructs/pointerFieldWidget.h \
    corestructs/widgetBlockHarness.h \
    corestructs/cameraModel/fixtureGlobalParametersWidget.h \
    framesources/imageCaptureInterfaceQt.h \
    corestructs/cameraModel/fixtureCameraControlWidget.h \
    corestructs/flowFabricControlWidget.h \

SOURCES += \
#    frames.cpp \
#    framesources/imageCaptureInterface.cpp \        # it uses WITH_DIRECTSHOW, WITH_UEYE, WITH_OPENCV
#    framesources/cameraControlParameters.cpp \
#    framesources/decoders/mjpegDecoder.cpp \
#    framesources/decoders/mjpegDecoderLazy.cpp \
#    framesources/decoders/decoupleYUYV.cpp \
#    framesources/decoders/aLowCodec.cpp \
    \
#    framesources/file/imageFileCaptureInterface.cpp \
#    framesources/file/abstractFileCapture.cpp \
#    framesources/file/abstractFileCaptureSpinThread.cpp \
    \
    fileformats/qtFileLoader.cpp \
    \
    flowDrawer.cpp \
    flowcolorers/abstractFlowColorer.cpp \
    flowcolorers/flowColorer.cpp \
    flowcolorers/stereoColorer.cpp \
    \
    timeliner.cpp \
    qtHelper.cpp \
    \
### visitors/defaultSetter.cpp \        # this file is obsolete version of the same name at "core/reflection"
    visitors/baseXMLVisitor.cpp \
    visitors/xmlSetter.cpp \
    visitors/xmlGetter.cpp \
    visitors/qSettingsSetter.cpp \
    visitors/qSettingsGetter.cpp \
#   visitors/printerVisitor.cpp \
    \
    serializer/serializer.cpp \
    serializer/serializedWidget.cpp \
    serializer/widgetQtIterator.cpp \
    \
    statistics/contentStatistics.cpp \
    statistics/userPoll.cpp \
    \
    rectifier/universalRectifier.cpp \
    rectifier/rectifyParameters.cpp \
    \
##  opengl/openGLTools.cpp \
    \
    camcalc/colorTimer.cpp \
    \
#    Generated \
    \
    widgets/generated/graphPlotParameters.cpp \
    \
    configManager.cpp \
    \
    corestructs/lockableObject.cpp \
    corestructs/g12Image.cpp \
    \
    visitors/jsonGetter.cpp \
    visitors/jsonSetter.cpp \
    widgets/vectorWidget.cpp \
    distortioncorrector/cameraModelParametersControlWidget.cpp \
    distortioncorrector/lensDistortionModelParametersControlWidget.cpp \
    distortioncorrector/calibrationFeaturesWidget.cpp \
    uis/cloudview/scene3dTreeView.cpp \
    corestructs/cameraModel/affine3dControlWidget.cpp \
    corestructs/cameraModel/fixtureControlWidget.cpp \
    widgets/observationListModel.cpp \
    distortioncorrector/pointListEditImageWidget.cpp \
    corestructs/cameraModel/featurePointControlWidget.cpp \
    uis/aboutPropsTableWidget.cpp \
    scannercontrol.cpp \
    uis/histogramDepthDialog.cpp \
    corestructs/reflectionWidget.cpp \
    corestructs/cameraModel/fixtureGeometryControlWidget.cpp \
    corestructs/pointerFieldWidget.cpp \
    corestructs/widgetBlockHarness.cpp \
    corestructs/cameraModel/fixtureGlobalParametersWidget.cpp \
    framesources/imageCaptureInterfaceQt.cpp \
    corestructs/cameraModel/fixtureCameraControlWidget.cpp \
    corestructs/flowFabricControlWidget.cpp \


FORMS += \
    widgets/vectorWidget.ui \
    distortioncorrector/cameraModelParametersControlWidget.ui \
    distortioncorrector/lensDistortionModelParametersControlWidget.ui \
    distortioncorrector/calibrationFeaturesWidget.ui \
#    tablecontrol/rotaryTableControlWidget.ui \
#    tablecontrol/rotationPlanGenerator.ui \
    corestructs/cameraModel/affine3dControlWidget.ui \
    corestructs/cameraModel/fixtureControlWidget.ui \
    corestructs/cameraModel/featurePointControlWidget.ui \
    corestructs/cameraModel/fixtureGeometryControlWidget.ui \
    corestructs/pointerFieldWidget.ui \
    corestructs/cameraModel/fixtureGlobalParametersWidget.ui \
    corestructs/cameraModel/fixtureCameraControlWidget.ui \
    corestructs/flowFabricControlWidget.ui

# =============================================================

# PREC
CONFIG += with_framesource_prec

# FILE
CONFIG += with_framesource_file


# =============================================================

HEADERS += memoryuse/memoryUsageCalculator.h
SOURCES += memoryuse/memoryUsageCalculator.cpp

win32 {
   HEADERS += memoryuse/windowsMemoryUsageCalculator.h
   SOURCES += memoryuse/windowsMemoryUsageCalculator.cpp
} else:macx {
   HEADERS += memoryuse/macMemoryUsageCalculator.h
   SOURCES += memoryuse/macMemoryUsageCalculator.cpp
} else {
   HEADERS += memoryuse/linuxMemoryUsageCalculator.h
   SOURCES += memoryuse/linuxMemoryUsageCalculator.cpp
}


# =============================================================



with_filters {
    include($$UTILSDIR/filters/ui/filterWidgets.pri)
    CONFIG += with_widgets


HEADERS += \
    filters/filterSelector.h \
    filters/filterExecuter.h \
    filters/filterParametersControlWidgetBase.h \
    filters/openCVFilter.h \
    \
    filters/graph/filterBlockPresentation.h \
    filters/graph/diagramitem.h \
    filters/graph/diagramscene.h \
    filters/graph/arrow.h \
    filters/graph/diagramtextitem.h \
    \
    filters/graph/filterGraphPresentation.h \

SOURCES += \
    filters/filterSelector.cpp \
    filters/filterExecuter.cpp \
    filters/openCVFilter.cpp \
    \
    filters/graph/filterBlockPresentation.cpp \
    filters/graph/diagramitem.cpp \
    filters/graph/arrow.cpp \
    filters/graph/diagramtextitem.cpp \
    filters/graph/diagramscene.cpp \
    \
    filters/graph/filterGraphPresentation.cpp \

FORMS += \
    filters/filterSelector.ui \
    \
    filters/graph/filterGraphPresentation.ui \

HEADERS += \
    filters/graph/filterPinPresentation.h \
    filters/graph/filterPresentationsCollection.h \
    filters/graph/inputBlockPresentation.h \
    filters/graph/outputBlockPresentation.h \
    filters/graph/g12PinPresentation.h \
    filters/graph/txtPinPresentation.h \
    filters/graph/compoundBlockPresentation.h \


SOURCES += \
    filters/graph/filterPinPresentation.cpp \
    filters/graph/filterPresentationsCollection.cpp \
    filters/graph/inputBlockPresentation.cpp \
    filters/graph/outputBlockPresentation.cpp \
    filters/graph/g12PinPresentation.cpp \
    filters/graph/txtPinPresentation.cpp \
    filters/graph/compoundBlockPresentation.cpp \

}


with_widgets {

include($$UTILSDIR/corestructs/coreWidgets/coreWidgets.pri)


HEADERS += \
    camcalc/cameraCalculatorWidget.h \
    \
    corestructs/parametersControlWidgetBase.h \
    corestructs/painterHelpers.h \
    corestructs/histogramwidget.h \
    corestructs/saveFlowSettings.h \
    \
    viAreaWidget.h \  
    matrixwidget.h \
    distortioncorrector/distortionWidget.h \
    \
    \
#    corestructs/libWidgets/openCVBMParameters.h \
#    corestructs/libWidgets/openCVSGMParameters.h \
#    corestructs/libWidgets/openCVBMParametersControlWidget.h \
#    corestructs/libWidgets/openCVSGMParametersControlWidget.h \
    \
    corestructs/histogramdialog.h \
    \
    rectifier/rectifyParametersControlWidget.h \
    \
    widgets/foldableWidget.h \
    widgets/exponentialSlider.h \
    widgets/parameterEditorWidget.h \
    widgets/parameterSlider.h \
    widgets/parameterSelector.h \
    widgets/inputSelectorWidget.h \
    widgets/loggerWidget.h \
    widgets/graphPlotParametersControlWidget.h \
    widgets/transform3DSelector.h \
    widgets/angleEditBox.h \
    widgets/shift3DWidget.h \
    \
    uis/advancedImageWidget.h \
    uis/paintImageWidget.h \
    uis/capSettingsDialog.h \
    uis/osdBaseWidget.h \
    uis/graphPlotDialog.h \
    uis/aboutDialog.h \
    uis/textLabelWidget.h \
    uis/pointsRectificationWidget.h \

SOURCES += \
    widgets/graphPlotParametersControlWidget.cpp \
    camcalc/cameraCalculatorWidget.cpp \
    \
    corestructs/histogramwidget.cpp \
    \
    corestructs/parametersControlWidgetBase.cpp \
    corestructs/histogramdialog.cpp \
    corestructs/painterHelpers.cpp \
    corestructs/saveFlowSettings.cpp \
    \
    viAreaWidget.cpp \
    matrixwidget.cpp \
    distortioncorrector/distortionWidget.cpp \
    \
#    filters/filterSelector.cpp \
#    filters/filterExecuter.cpp \
#    filters/openCVFilter.cpp \
    \
#    filters/graph/filterBlockPresentation.cpp \
#    filters/graph/diagramitem.cpp \
#    filters/graph/arrow.cpp \
#    filters/graph/diagramtextitem.cpp \
#    filters/graph/diagramscene.cpp \
    \
#    corestructs/libWidgets/openCVBMParameters.cpp \
#    corestructs/libWidgets/openCVSGMParameters.cpp \
#    corestructs/libWidgets/openCVBMParametersControlWidget.cpp \
#    corestructs/libWidgets/openCVSGMParametersControlWidget.cpp \
    \
    rectifier/rectifyParametersControlWidget.cpp \
    \
    widgets/foldableWidget.cpp \
    widgets/exponentialSlider.cpp \
    widgets/parameterEditorWidget.cpp \
    widgets/parameterSlider.cpp \
    widgets/parameterSelector.cpp \
    widgets/inputSelectorWidget.cpp \
    widgets/loggerWidget.cpp \
    widgets/transform3DSelector.cpp \
    widgets/angleEditBox.cpp \
    widgets/shift3DWidget.cpp \
    \
    uis/advancedImageWidget.cpp \
    uis/paintImageWidget.cpp \
    uis/capSettingsDialog.cpp \
    uis/osdBaseWidget.cpp \
    uis/graphPlotDialog.cpp \
    uis/aboutDialog.cpp \
    uis/textLabelWidget.cpp \
    uis/pointsRectificationWidget.cpp \



FORMS += \
    \
    corestructs/histogramdialog.ui \
    corestructs/histogramwidget.ui \
    corestructs/saveFlowSettings.ui \
    \
    viAreaWidget.ui \
    matrixwidget.ui \
    \
    widgets/foldableWidget.ui \
    widgets/exponentialSlider.ui \
    widgets/parameterSlider.ui \
    widgets/parameterSelector.ui \
    widgets/loggerWidget.ui \
    widgets/inputSelectorWidget.ui \
    widgets/graphPlotParametersControlWidget.ui \
    widgets/transform3DSelector.ui \
    widgets/angleEditBox.ui \
    widgets/shift3DWidget.ui \
    \
#    corestructs/libWidgets/openCVBMParametersControlWidget.ui \
#    corestructs/libWidgets/openCVSGMParametersControlWidget.ui \
    \
#    filters/filterSelector.ui \
    \
    \
    camcalc/cameraCalculatorWidget.ui \
    \
    uis/advancedImageWidget.ui \
    uis/paintImageWidget.ui \
    uis/histogramDepthDialog.ui \
    uis/capSettingsDialog.ui \
    uis/graphPlotDialog.ui \
    uis/aboutDialog.ui \
    uis/textLabelWidget.ui \
    uis/pointsRectificationWidget.ui \
    \
    rectifier/rectifyParametersControlWidget.ui \
    distortioncorrector/distortionWidget.ui \


}

RESOURCES += \
   ../resources/main.qrc

unix:!macx:!win32 {
    CONFIG += with_v4l2
}

with_v4l2 {
    V4L2_WRAPPER_DIR = $$UTILSDIR/../wrappers/v4l2
    include($$V4L2_WRAPPER_DIR/v4l2.pri)                # it uses OPENCV_WRAPPER_DIR inside
}



with_opengl {
    DEPENDPATH  += opengl
    DEPENDPATH  += uis/cloudview
    INCLUDEPATH += uis/cloudview

    HEADERS     += opengl/openGLTools.h
    SOURCES     += opengl/openGLTools.cpp

    HEADERS     += \
         uis/cloudview/cloudViewDialog.h \
         uis/cloudview/treeSceneController.h \
         3d/scene3D.h \
         \
         3d/draw3dParametersControlWidget.h \
         3d/draw3dCameraParametersControlWidget.h \
         3d/draw3dViMouseParametersControlWidget.h \
         \
         3d/generated/draw3dParameters.h \
         3d/generated/draw3dStyle.h \
         3d/generated/draw3dCameraParameters.h \
         3d/generated/viMouse3DStereoStyle.h \
         3d/generated/viMouse3DFlowStyle.h \
         3d/generated/viMouse3DStereoStyle.h \
         3d/generated/viMouse3DFlowStyle.h \
         3d/generated/draw3dViMouseParameters.h \
         3d/mesh3DScene.h \
         3d/coordinateFrame.h \
         3d/helper3DScenes.h \
         \


    SOURCES += \
        3d/scene3D.cpp \
        \
        uis/cloudview/cloudViewDialog.cpp \
        uis/cloudview/treeSceneController.cpp \
        3d/generated/draw3dParameters.cpp \
        3d/draw3dParametersControlWidget.cpp \
        3d/draw3dCameraParametersControlWidget.cpp \
        3d/generated/draw3dCameraParameters.cpp \
        3d/draw3dViMouseParametersControlWidget.cpp \
        3d/generated/draw3dViMouseParameters.cpp \
        \
        3d/mesh3DScene.cpp \
        3d/coordinateFrame.cpp \
        3d/helper3DScenes.cpp

    FORMS       += \
        uis/cloudview/cloudViewDialog.ui \
        #   Generated    \
        \
        3d/draw3dParametersControlWidget.ui \
        3d/draw3dCameraParametersControlWidget.ui \
        3d/draw3dViMouseParametersControlWidget.ui \

#some dependancies


    HEADERS     +=  viGLAreaWidget.h
    SOURCES     +=  viGLAreaWidget.cpp
    FORMS       +=  viGLAreaWidget.ui


#    HEADERS     +=  tablecontrol/rotaryTableControlWidget.h
#    SOURCES     +=  tablecontrol/rotaryTableControlWidget.cpp
#    FORMS       +=  tablecontrol/rotaryTableControlWidget.ui


    SOURCES     +=  3d/sceneShaded.cpp \
                    3d/billboardCaption3DScene.cpp \
                    3d/gCodeScene.cpp \
                    3d/shadedSceneControlWidget.cpp \

    HEADERS     +=  3d/sceneShaded.h \
                    3d/billboardCaption3DScene.h \
                    3d/gCodeScene.h \
                    3d/shadedSceneControlWidget.h \

    FORMS       +=  3d/shadedSceneControlWidget.ui \



}

with_ueye {
    contains(DEFINES, WITH_UEYE) {
        HEADERS += \
            framesources/uEye/uEyeCapture.h \
            framesources/uEye/uEyeCameraDescriptor.h \

        SOURCES += \
            framesources/uEye/uEyeCapture.cpp \
            framesources/uEye/uEyeCameraDescriptor.cpp \

        INCLUDEPATH += framesources/uEye
    }
}

with_flycap {
    contains(DEFINES, WITH_FLYCAP) {

        INCLUDEPATH += $$[QT_SYSROOT]/usr/include/flycapture
        INCLUDEPATH += framesources/flycap

        HEADERS += framesources/flycap/flyCapCapture.h
        SOURCES += framesources/flycap/flyCapCapture.cpp
    }

}


with_opencv {
    OPENCV_WRAPPER_DIR = $$UTILSDIR/../wrappers/opencv
    include($$OPENCV_WRAPPER_DIR/opencv.pri)                # it uses OPENCV_WRAPPER_DIR inside

    contains(DEFINES, WITH_OPENCV) {                        # TODO: move this to OpenCV
        HEADERS += \
            framesources/opencv/openCVCapture.h \
            framesources/opencv/openCVFileCapture.h \
            framesources/opencv/openCVHelper.h \

        SOURCES += \
            framesources/opencv/openCVCapture.cpp \
            framesources/opencv/openCVFileCapture.cpp \
            framesources/opencv/openCVHelper.cpp \

    }
}

with_rapidjson {
    RAPIDJSON_WRAPPER_DIR = $$UTILSDIR/../wrappers/rapidjson
    include($$RAPIDJSON_WRAPPER_DIR/rapidjson.pri)

    contains(DEFINES, WITH_RAPIDJSON) {
       #HEADERS +=  $$RAPIDJSON_WRAPPER_DIR/rapidJSONReader.h
       #SOURCES +=  $$RAPIDJSON_WRAPPER_DIR/rapidJSONReader.cpp
       #HEADERS +=  $$RAPIDJSON_WRAPPER_DIR/rapidJSONWriter.h
       #SOURCES +=  $$RAPIDJSON_WRAPPER_DIR/rapidJSONWriter.cpp
    }
}

with_jsonmodern {
    JSONMODERN_WRAPPER_DIR = $$UTILSDIR/../wrappers/jsonmodern
    include($$JSONMODERN_WRAPPER_DIR/jsonmodern.pri)

    contains(DEFINES, WITH_JSONMODERN) {
        # HEADERS += $$JSONMODERN_WRAPPER_DIR/jsonModernReader.h
        # SOURCES += $$JSONMODERN_WRAPPER_DIR/jsonModernReader.cpp
    }
}

with_siftgpu {
        DEFINES += WITH_SIFTGPU
        SIFTGPU_WRAPPER_DIR = $$UTILSDIR/../wrappers/siftgpu
        include($$SIFTGPU_WRAPPER_DIR/siftgpu.pri)
}


with_directshow {
    HEADERS += \
        framesources/directShow/directShowCapture.h \
        framesources/directShow/directShowCaptureDecouple.h \
        framesources/directShow/directShow.h \

    SOURCES += \
        framesources/directShow/directShowCapture.cpp \
        framesources/directShow/directShowCaptureDecouple.cpp \
        framesources/directShow/directShow.cpp \

    DEFINES += WITH_DIRECTSHOW
}


with_avcodec {
    AVCODEC_WRAPPER_DIR = $$UTILSDIR/../wrappers/avcodec
    include($$AVCODEC_WRAPPER_DIR/avcodec.pri)
}

with_synccam {
    HEADERS += framesources/syncCam/syncCamerasCaptureInterface.h
    SOURCES += framesources/syncCam/syncCamerasCaptureInterface.cpp

    DEFINES += WITH_SYNCCAM
}


with_qscript {
    SOURCES += scripting/scriptWindow.cpp
    HEADERS += scripting/scriptWindow.h
    FORMS   += scripting/scriptWindow.ui
}

OTHER_FILES += ../tools/generator/xml/draw3d.xml

#message($$CONFIG)
