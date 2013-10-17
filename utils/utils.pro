# try use global config 
exists(../../../config.pri) {
    ROOT_DIR=../../..
    #message(Using global config)
} else { 
    message(Using local config)
    ROOT_DIR=..
}
ROOT_DIR=$$PWD/$$ROOT_DIR
include($$ROOT_DIR/config.pri)


CONFIG  += staticlib
TARGET   = cvs_utils
TEMPLATE = lib

# Utils lib uses core, core-res. These includes must be before adding utils lib for mingw linker!
#
# But they create unneccessary dependence core->utils, although they are not linked together, but should!
#
#COREDIR = ../../core
#include(../../core/core.pri)                       # it uses COREDIR, TARGET and detects     COREBINDIR!
#include(../../core/core_restricted.pri)            # it uses COREDIR, TARGET and detects RES_COREBINDIR!

COREDIR=../core

UTILSDIR=$$PWD
include($$UTILSDIR/utils.pri)                      # it uses UTILSDIR, TARGET and detects UTILS_BINDIR, OBJECTS_DIR,...!

include($$UTILSDIR/corestructs/coreWidgets/coreWidgets.pri)
include($$UTILSDIR/filters/ui/filterWidgets.pri)


HEADERS += \
    3d/scene3D.h \
    \
    corestructs/parametersControlWidgetBase.h \
    corestructs/g12Image.h \
    corestructs/painterHelpers.h \
    corestructs/histogramdialog.h \
    corestructs/histogramwidget.h \
    corestructs/saveFlowSettings.h \
    \
    corestructs/libWidgets/openCVBMParameters.h \
    corestructs/libWidgets/openCVSGMParameters.h \
    corestructs/libWidgets/openCVBMParametersControlWidget.h \
    corestructs/libWidgets/openCVSGMParametersControlWidget.h \
    \
    viAreaWidget.h \
    viGLAreaWidget.h \
    matrixwidget.h \
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
    camcalc/cameraCalculatorWidget.h \
    \
    frames.h \
    framesources/imageCaptureInterface.h \
    framesources/imageFileCaptureInterface.h \
    framesources/fileCapture.h \
    framesources/abstractFileCapture.h \
    framesources/abstractFileCaptureSpinThread.h \
    framesources/cameraControlParameters.h \
    framesources/decoders/mjpegDecoder.h \
    framesources/decoders/mjpegDecoderLazy.h \
    framesources/decoders/decoupleYUYV.h \
    \
    fileformats/qtFileLoader.h \
    uis/advancedImageWidget.h \
    uis/paintImageWidget.h \    
    uis/histogramdepthdialog.h \
    uis/capSettingsDialog.h \
    uis/osdBaseWidget.h \
    uis/graphPlotDialog.h \
    uis/aboutDialog.h \
    uis/textLabelWidget.h \
    uis/pointsRectificationWidget.h \
    \
    flowDrawer.h \
    flowcolorers/abstractFlowColorer.h \
    flowcolorers/flowColorer.h \
    flowcolorers/stereoColorer.h \
    \
    qtHelper.h \
    timeliner.h \
    \
    visitors/xmlSetter.h \
    visitors/qSettingsSetter.h \
    visitors/qSettingsGetter.h \
    visitors/xmlGetter.h \
    \
    serializer/serializer.h \
    serializer/serializedWidget.h \
    serializer/widgetQtIterator.h \
    \
    statistics/contentStatistics.h \
    \
    rectifier/estimationMethodType.h \
    rectifier/optimizationMethodType.h \
    rectifier/universalRectifier.h \
    rectifier/rectifyParametersControlWidget.h \
    rectifier/rectifyParameters.h \
    \
##  opengl/openGLTools.h \
    \
    distortioncorrector/distortionWidget.h \
    distortioncorrector/distortionParameters.h \
    camcalc/colorTimer.h \
    \
#    Generated    \
    \
    widgets/generated/graphPlotParameters.h \
    widgets/generated/graphStyle.h \
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
    filters/graph/filterPinPresentation.h \
    filters/graph/filterGraphPresentation.h \
    filters/graph/filterPresentationsCollection.h \
    filters/graph/inputBlockPresentation.h \
    filters/graph/outputBlockPresentation.h \
    filters/graph/g12PinPresentation.h \
    filters/graph/txtPinPresentation.h \
    filters/graph/compoundBlockPresentation.h \
    configManager.h
  

SOURCES += \
    3d/scene3D.cpp \
    \
    corestructs/parametersControlWidgetBase.cpp \    
    corestructs/histogramdialog.cpp \
    corestructs/histogramwidget.cpp \
    corestructs/g12Image.cpp \
    corestructs/painterHelpers.cpp \
    corestructs/saveFlowSettings.cpp \
    \
    corestructs/libWidgets/openCVBMParameters.cpp \
    corestructs/libWidgets/openCVSGMParameters.cpp \
    corestructs/libWidgets/openCVBMParametersControlWidget.cpp \
    corestructs/libWidgets/openCVSGMParametersControlWidget.cpp \  
    \
    viAreaWidget.cpp \
    viGLAreaWidget.cpp \
    matrixwidget.cpp \
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
    camcalc/cameraCalculatorWidget.cpp \
    \
    frames.cpp \
    framesources/imageCaptureInterface.cpp \        # it uses WITH_DIRECTSHOW, WITH_UEYE, WITH_OPENCV
    framesources/imageFileCaptureInterface.cpp \
    framesources/abstractFileCapture.cpp \
    framesources/abstractFileCaptureSpinThread.cpp \
    framesources/cameraControlParameters.cpp \
    framesources/fileCapture.cpp \
    framesources/decoders/mjpegDecoder.cpp \
    framesources/decoders/mjpegDecoderLazy.cpp \
    framesources/decoders/decoupleYUYV.cpp \
    \
    fileformats/qtFileLoader.cpp \
    uis/advancedImageWidget.cpp \
    uis/paintImageWidget.cpp \    
    uis/histogramdepthdialog.cpp \
    uis/capSettingsDialog.cpp \
    uis/osdBaseWidget.cpp \   
    uis/graphPlotDialog.cpp \
    uis/aboutDialog.cpp \
    uis/textLabelWidget.cpp \
    uis/pointsRectificationWidget.cpp \
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
    visitors/xmlSetter.cpp \
    visitors/qSettingsSetter.cpp \
    visitors/qSettingsGetter.cpp \
    visitors/xmlGetter.cpp \
#   visitors/printerVisitor.cpp \
    \
    serializer/serializer.cpp \
    serializer/serializedWidget.cpp \
    serializer/widgetQtIterator.cpp \
    \
    statistics/contentStatistics.cpp \
    \
    rectifier/universalRectifier.cpp \
    rectifier/rectifyParametersControlWidget.cpp \
    rectifier/rectifyParameters.cpp \
    \
##  opengl/openGLTools.cpp \
    \
    distortioncorrector/distortionWidget.cpp \
    camcalc/colorTimer.cpp \
    \
#    Generated \
    \
    widgets/generated/graphPlotParameters.cpp \
    widgets/generated/graphPlotParametersControlWidget.cpp \
    \
    3d/generated/draw3dParameters.cpp \
    3d/generated/draw3dParametersControlWidget.cpp \
    3d/generated/draw3dCameraParametersControlWidget.cpp \
    3d/generated/draw3dCameraParameters.cpp \
    3d/generated/draw3dViMouseParametersControlWidget.cpp \
    3d/generated/draw3dViMouseParameters.cpp \
    \
    3d/mesh3DScene.cpp \
    3d/coordinateFrame.cpp \
    filters/graph/filterPinPresentation.cpp \
    filters/graph/filterGraphPresentation.cpp \
    filters/graph/filterPresentationsCollection.cpp \
    filters/graph/inputBlockPresentation.cpp \
    filters/graph/outputBlockPresentation.cpp \
    filters/graph/g12PinPresentation.cpp \
    filters/graph/txtPinPresentation.cpp \
    filters/graph/compoundBlockPresentation.cpp \
    configManager.cpp
 

FORMS += \
    \
    corestructs/histogramdialog.ui \
    corestructs/histogramwidget.ui \
    corestructs/saveFlowSettings.ui \
    \
    viAreaWidget.ui \
    viGLAreaWidget.ui \
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
    corestructs/libWidgets/openCVBMParametersControlWidget.ui \
    corestructs/libWidgets/openCVSGMParametersControlWidget.ui \
    \
    filters/filterSelector.ui \
    \
    \
    camcalc/cameraCalculatorWidget.ui \
    \
    uis/advancedImageWidget.ui \
    paintImageWidget.ui \    
    uis/histogramdepthdialog.ui \
    uis/capSettingsDialog.ui \
    uis/graphPlotDialog.ui \
    uis/aboutDialog.ui \
    uis/textLabelWidget.ui \
    uis/pointsRectificationWidget.ui \
    \
    rectifier/rectifyParametersControlWidget.ui \
    distortioncorrector/distortionWidget.ui \    
    \
#   Generated    \
    \
    3d/generated/draw3dParametersControlWidget.ui \
    3d/generated/draw3dCameraParametersControlWidget.ui \
    3d/generated/draw3dViMouseParametersControlWidget.ui \
    filters/graph/filterGraphPresentation.ui \

    

unix:!macx {
    HEADERS += \
        framesources/v4l2/V4L2.h \
        framesources/v4l2/V4L2Capture.h \
        framesources/v4l2/V4L2CaptureDecouple.h \

    SOURCES += \
        framesources/v4l2/V4L2.cpp \
        framesources/v4l2/V4L2Capture.cpp \
        framesources/v4l2/V4L2CaptureDecouple.cpp \

}

with_opengl {
    DEPENDPATH  += opengl 
    DEPENDPATH  += uis/cloudview
    INCLUDEPATH += uis/cloudview
    

    HEADERS     += opengl/openGLTools.h
    SOURCES     += opengl/openGLTools.cpp

    HEADERS     += uis/cloudview/cloudViewDialog.h \ 
    			   uis/cloudview/treeSceneController.h \
    			   
    SOURCES     += uis/cloudview/cloudViewDialog.cpp \ 
                   uis/cloudview/treeSceneController.cpp \
                       
    FORMS       += uis/cloudview/cloudViewDialog.ui
}

with_ueye {
    contains(DEFINES, WITH_UEYE) {
        HEADERS += \
            framesources/uEyeCapture.h \
            framesources/uEyeCameraDescriptor.h \

        SOURCES += \
            framesources/uEyeCapture.cpp \
            framesources/uEyeCameraDescriptor.cpp \
    }
}

with_opencv {
    OPENCV_WRAPPER_DIR = $$UTILSDIR/../wrappers/opencv
    include($$OPENCV_WRAPPER_DIR/opencv.pri)            # it uses OPENCV_WRAPPER_DIR inside

    contains(DEFINES, WITH_OPENCV) {  # Move this to OpenCV
        HEADERS += \
            framesources/openCVCapture.h \
            framesources/openCvFileCapture.h \
            framesources/openCvHelper.h \

        SOURCES += \
            framesources/openCVCapture.cpp \
            framesources/openCvFileCapture.cpp \
            framesources/openCvHelper.cpp \

    }
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
