cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME cvs_utils)
init_project(PROJECT_NAME corecvs_utils)

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script Xml SerialPort)

set(PUBLIC_HEADER_FILES
    configManager.h
    flowDrawer.h
    matrixwidget.h
    qtHelper.h
    scannercontrol.h
    timeliner.h
    trackPainter.h
    viAreaWidget.h
    viGLAreaWidget.h
    )

set(CAMCALC_HEADER_FILES
    camcalc/cameraCalculatorWidget.h
    camcalc/colorTimer.h
    )

set(CORESTRUCTS_HEADER_FILES
    corestructs/flowFabricControlWidget.h
    corestructs/g12Image.h
    corestructs/histogramdialog.h
    corestructs/histogramwidget.h
    corestructs/lockableObject.h
    corestructs/painterHelpers.h
    corestructs/parametersControlWidgetBase.h
    corestructs/pointerFieldWidget.h
    corestructs/reflectionWidget.h
    corestructs/saveFlowSettings.h
    corestructs/universalVisitor.h
    corestructs/widgetBlockHarness.h
    corestructs/zoomablearea.h
    corestructs/cameraModel/affine3dControlWidget.h
    corestructs/cameraModel/featurePointControlWidget.h
    corestructs/cameraModel/fixtureCameraControlWidget.h
    corestructs/cameraModel/fixtureControlWidget.h
    corestructs/cameraModel/fixtureGeometryControlWidget.h
    corestructs/cameraModel/fixtureGlobalParametersWidget.h
    corestructs/coreWidgets/axisAlignedBoxParametersControlWidget.h
    corestructs/coreWidgets/bitcodeBoardParamsBaseControlWidget.h
    corestructs/coreWidgets/checkerboardDetectionParametersControlWidget.h
    corestructs/coreWidgets/chessBoardAssemblerParamsBaseControlWidget.h
    corestructs/coreWidgets/chessBoardCornerDetectorParamsBaseControlWidget.h
    corestructs/coreWidgets/debayerParametersControlWidget.h
    corestructs/coreWidgets/distortionApplicationParametersControlWidget.h
    corestructs/coreWidgets/euclidianMoveParametersControlWidget.h
    corestructs/coreWidgets/headSearchParametersControlWidget.h
    corestructs/coreWidgets/lineDistortionEstimatorParametersControlWidget.h
    corestructs/coreWidgets/makePreciseParametersControlWidget.h
    corestructs/coreWidgets/rgbColorParametersControlWidget.h
    )

set(DISTORTIONCORRECTOR_HEADER_FILES
    distortioncorrector/calibrationFeaturesWidget.h
    distortioncorrector/cameraModelParametersControlWidget.h
    distortioncorrector/distortionCorrectionDoc.h
    distortioncorrector/distortionParameters.h
    distortioncorrector/distortionWidget.h
    distortioncorrector/lensDistortionModelParametersControlWidget.h
    distortioncorrector/pointListEditImageWidget.h
    )

set(FILEFORMATS_HEADER_FILE
    fileformats/qtFileLoader.h
    )

set(FLOWCOLORERS_HEADER_FILES
    flowcolorers/abstractFlowColorer.h
    flowcolorers/flowColorer.h
    flowcolorers/stereoColorer.h
    )

set(FRAMESOURCES_HEADER_FILES
    framesources/imageCaptureInterfaceQt.h
    framesources/directShow/directShow.h
    framesources/directShow/directShowCapture.h
    framesources/directShow/directShowCaptureDecouple.h
    framesources/opencv/openCVCapture.h
    framesources/opencv/openCVFileCapture.h
    framesources/opencv/openCVHelper.h
    framesources/uEye/uEyeCameraDescriptor.h
    framesources/uEye/uEyeCapture.h
    )

set(RECTIFIER_HEADER_FILES
    rectifier/estimationMethodType.h
    rectifier/matchingMethodType.h
    rectifier/optimizationMethodType.h
    rectifier/rectifyParameters.h
    rectifier/rectifyParametersControlWidget.h
    rectifier/universalRectifier.h
    )

set(SCRIPTING_HEADER_FILES
    scripting/coreToScript.h
    scripting/scriptWindow.h
    )

set(SERIALIZER_HEADER_FILES
    serializer/serializedWidget.h
    serializer/serializer.h
    serializer/widgetQtIterator.h
    )

set(STATISTICS_HEADER_FILES
    statistics/contentStatistics.h
    statistics/userPoll.h
    )

set(UIS_HEADER_FILES
    uis/aboutDialog.h
    uis/aboutPropsTableWidget.h
    uis/advancedImageWidget.h
    uis/capSettingsDialog.h
    uis/graphPlotDialog.h
    uis/histogramDepthDialog.h
    uis/osdBaseWidget.h
    uis/paintImageWidget.h
    uis/pointsRectificationWidget.h
    uis/textLabelWidget.h
    )

set(VISITORS_HEADER_FILES
    visitors/baseXMLVisitor.h
    visitors/defaultSetterOld.h
    visitors/jsonGetter.h
    visitors/jsonSetter.h
    visitors/qSettingsGetter.h
    visitors/qSettingsSetter.h
    visitors/xmlGetter.h
    visitors/xmlSetter.h
    )

set(WIDGETS_HEADER_FILES
    widgets/angleEditBox.h
    widgets/exponentialSlider.h
    widgets/foldableWidget.h
    widgets/graphPlotParametersControlWidget.h
    widgets/inputSelectorWidget.h
    widgets/loggerWidget.h
    widgets/observationListModel.h
    widgets/parameterEditorWidget.h
    widgets/parameterSelector.h
    widgets/parameterSlider.h
    widgets/shift3dWidget.h
    widgets/transform3DSelector.h
    widgets/vectorWidget.h
    widgets/generated/graphPlotParameters.h
    widgets/generated/graphStyle.h
    )

set(IF_OPENGL_HEADER_FILES)
set(3D_HEADER_FILES)
set(OPENGL_HEADER_FILES)
set(UIS_CLOUVIEW_HEADER_FILES)

option(WITH_OPENGL "Should use OpenGL from Qt" YES)

if(WITH_OPENGL)
    set(3D_HEADER_FILES
        ${3D_HEADER_FILES}
        3d/billboardCaption3DScene.h
        3d/coordinateFrame.h
        3d/draw3dCameraParametersControlWidget.h
        3d/draw3dParametersControlWidget.h
        3d/draw3dViMouseParametersControlWidget.h
        3d/gCodeScene.h
        3d/helper3DScenes.h
        3d/mesh3DScene.h
        3d/scene3D.h
        3d/sceneShaded.h
        3d/shadedSceneControlWidget.h
        3d/generated/draw3dCameraParameters.h
        3d/generated/draw3dParameters.h
        3d/generated/draw3dStyle.h
        3d/generated/draw3dTextureGen.h
        3d/generated/draw3dViMouseParameters.h
        3d/generated/viMouse3dFlowStyle.h
        3d/generated/viMouse3dStereoStyle.h
        )
    set(OPENGL_HEADER_FILES
        ${OPENGL_HEADER_FILES}
        opengl/openGLTools.h
        )
    set(UIS_CLOUVIEW_HEADER_FILES
        ${UIS_CLOUVIEW_HEADER_FILES}
        uis/cloudview/cloudViewDialog.h
        uis/cloudview/scene3dTreeView.h
        uis/cloudview/treeSceneController.h
        )     
endif()

set(IF_OPENGL_HEADER_FILES
    ${IF_OPENGL_HEADER_FILES}
    ${3D_HEADER_FILES}
    ${OPENGL_HEADER_FILES}
    ${UIS_CLOUVIEW_HEADER_FILES}
    )   

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    ${CAMCALC_HEADER_FILES}
    ${CORESTRUCTS_HEADER_FILES}
    ${DISTORTIONCORRECTOR_HEADER_FILES}
    ${FILEFORMATS_HEADER_FILE}
    ${FLOWCOLORERS_HEADER_FILES}
    ${FRAMESOURCES_HEADER_FILES}
    ${RECTIFIER_HEADER_FILES}
    ${SCRIPTING_HEADER_FILES}
    ${SERIALIZER_HEADER_FILES}
    ${STATISTICS_HEADER_FILES}
    ${UIS_HEADER_FILES}
    ${VISITORS_HEADER_FILES}
    ${WIDGETS_HEADER_FILES}
    ${IF_OPENGL_HEADER_FILES}
    )

set(SOURCE_FILES
    configManager.cpp
    flowDrawer.cpp
    matrixwidget.cpp
    qtHelper.cpp
    scannercontrol.cpp
    timeliner.cpp
    trackPainter.cpp
    viAreaWidget.cpp
    viGLAreaWidget.cpp
    )

set(CAMCALC_SOURCE_FILES
    camcalc/cameraCalculatorWidget.cpp
    camcalc/colorTimer.cpp
    )

set(CORESTRUCTS_HEADER_FILES
    corestructs/flowFabricControlWidget.cpp
    corestructs/g12Image.cpp
    corestructs/histogramdialog.cpp
    corestructs/histogramwidget.cpp
    corestructs/lockableObject.cpp
    corestructs/painterHelpers.cpp
    corestructs/parametersControlWidgetBase.cpp
    corestructs/pointerFieldWidget.cpp
    corestructs/reflectionWidget.cpp
    corestructs/saveFlowSettings.cpp
    corestructs/universalVisitor.cpp
    corestructs/widgetBlockHarness.cpp
    corestructs/zoomablearea.cpp
    corestructs/cameraModel/affine3dControlWidget.cpp
    corestructs/cameraModel/featurePointControlWidget.cpp
    corestructs/cameraModel/fixtureCameraControlWidget.cpp
    corestructs/cameraModel/fixtureControlWidget.cpp
    corestructs/cameraModel/fixtureGeometryControlWidget.cpp
    corestructs/cameraModel/fixtureGlobalParametersWidget.cpp
    corestructs/coreWidgets/adderSubstractorParametersBaseControlWidget.cpp
    corestructs/coreWidgets/axisAlignedBoxParametersControlWidget.cpp
    corestructs/coreWidgets/bitcodeBoardParamsBaseControlWidget.cpp
    corestructs/coreWidgets/calibrationDrawHelpersParametersControlWidget.cpp
    corestructs/coreWidgets/checkerboardDetectionParametersControlWidget.cpp
    corestructs/coreWidgets/chessBoardAssemblerParamsBaseControlWidget.cpp
    corestructs/coreWidgets/chessBoardCornerDetectorParamsBaseControlWidget.cpp
    corestructs/coreWidgets/debayerParametersControlWidget.cpp
    corestructs/coreWidgets/distortionApplicationParametersControlWidget.cpp
    corestructs/coreWidgets/euclidianMoveParametersControlWidget.cpp
    corestructs/coreWidgets/headSearchParametersControlWidget.cpp
    corestructs/coreWidgets/homorgaphyReconstructorBlockBaseControlWidget.cpp
    corestructs/coreWidgets/iterativeEstimateParametersControlWidget.cpp
    corestructs/coreWidgets/lensDistortionModelParametersBaseControlWidget.cpp
    corestructs/coreWidgets/lineDistortionEstimatorParametersControlWidget.cpp
    corestructs/coreWidgets/makePreciseParametersControlWidget.cpp
    corestructs/coreWidgets/ransacParametersControlWidget.cpp
    corestructs/coreWidgets/rgbColorParametersControlWidget.cpp
    corestructs/coreWidgets/sceneStereoAlignerBlockBaseControlWidget.cpp
    )

set(DISTORTIONCORRECTOR_SOURCE_FILES
    distortioncorrector/calibrationFeaturesWidget.cpp
    distortioncorrector/cameraModelParametersControlWidget.cpp
    distortioncorrector/distortionWidget.cpp
    distortioncorrector/lensDistortionModelParametersControlWidget.cpp
    distortioncorrector/pointListEditImageWidget.cpp
    )

set(FILEFORMATS_SOURCE_FILE
    fileformats/qtFileLoader.cpp
    )

set(FLOWCOLORERS_SOURCE_FILES
    flowcolorers/abstractFlowColorer.cpp
    flowcolorers/flowColorer.cpp
    flowcolorers/stereoColorer.cpp
    )

set(FRAMESOURCES_SOURCE_FILES
    framesources/imageCaptureInterfaceQt.cpp
    framesources/directShow/directShow.cpp
    framesources/directShow/directShowCapture.cpp
    framesources/directShow/directShowCaptureDecouple.cpp
    framesources/opencv/openCVCapture.cpp
    framesources/opencv/openCVFileCapture.cpp
    framesources/opencv/openCVHelper.cpp
    framesources/uEye/uEyeCameraDescriptor.cpp
    framesources/uEye/uEyeCapture.cpp
    )

set(RECTIFIER_SOURCE_FILES
    rectifier/rectifyParameters.cpp
    rectifier/rectifyParametersControlWidget.cpp
    rectifier/universalRectifier.cpp
    )

set(SCRIPTING_SOURCE_FILE
    scripting/scriptWindow.cpp
    )

set(SERIALIZER_SOURCE_FILES
    serializer/serializedWidget.cpp
    serializer/serializer.cpp
    serializer/widgetQtIterator.cpp
    )

set(STATISTICS_SOURCE_FILES
    statistics/contentStatistics.cpp
    statistics/userPoll.cpp
    )

set(UIS_SOURCE_FILES
    uis/aboutDialog.cpp
    uis/aboutPropsTableWidget.cpp
    uis/advancedImageWidget.cpp
    uis/capSettingsDialog.cpp
    uis/graphPlotDialog.cpp
    uis/histogramDepthDialog.cpp
    uis/osdBaseWidget.cpp
    uis/paintImageWidget.cpp
    uis/pointsRectificationWidget.cpp
    uis/textLabelWidget.cpp
    )

set(VISITORS_SOURCE_FILES
    visitors/baseXMLVisitor.cpp
    visitors/defaultSetterOld.cpp
    visitors/jsonGetter.cpp
    visitors/jsonSetter.cpp
    visitors/qSettingsGetter.cpp
    visitors/qSettingsSetter.cpp
    visitors/xmlGetter.cpp
    visitors/xmlSetter.cpp
    )

set(WIDGETS_SOURCE_FILES
    widgets/angleEditBox.cpp
    widgets/exponentialSlider.cpp
    widgets/foldableWidget.cpp
    widgets/graphPlotParametersControlWidget.cpp
    widgets/inputSelectorWidget.cpp
    widgets/loggerWidget.cpp
    widgets/observationListModel.cpp
    widgets/parameterEditorWidget.cpp
    widgets/parameterSelector.cpp
    widgets/parameterSlider.cpp
    widgets/shift3dWidget.cpp
    widgets/transform3DSelector.cpp
    widgets/vectorWidget.cpp
    widgets/generated/graphPlotParameters.cpp
    )

set(IF_OPENGL_SOURCE_FILES)
set(3D_SOURCE_FILES)
set(OPENGL_SOURCE_FILES)
set(UIS_CLOUVIEW_SOURCE_FILES)

if(WITH_OPENGL)
    set(3D_SOURCE_FILES
        ${3D_SOURCE_FILES}
        3d/billboardCaption3DScene.cpp
        3d/coordinateFrame.cpp
        3d/draw3dCameraParametersControlWidget.cpp
        3d/draw3dParametersControlWidget.cpp
        3d/draw3dViMouseParametersControlWidget.cpp
        3d/gCodeScene.cpp
        3d/helper3DScenes.cpp
        3d/mesh3DScene.cpp
        3d/scene3D.cpp
        3d/sceneShaded.cpp
        3d/shadedSceneControlWidget.cpp
        3d/generated/draw3dCameraParameters.cpp
        3d/generated/draw3dParameters.cpp
        3d/generated/draw3dViMouseParameters.cpp
        )
    set(OPENGL_SOURCE_FILES
        ${OPENGL_SOURCE_FILES}
        opengl/openGLTools.cpp
        )
    set(UIS_CLOUVIEW_SOURCE_FILES
        ${UIS_CLOUVIEW_SOURCE_FILES}
        uis/cloudview/cloudViewDialog.cpp
        uis/cloudview/scene3dTreeView.cpp
        uis/cloudview/treeSceneController.cpp
        )
endif()

set(IF_OPENGL_SOURCE_FILES
    ${IF_OPENGL_SOURCE_FILES}
    ${3D_SOURCE_FILES}
    ${OPENGL_SOURCE_FILES}
    ${UIS_CLOUVIEW_SOURCE_FILES}
    )

set(SOURCES
    ${SOURCE_FILES}
    ${CAMCALC_SOURCE_FILES}
    ${CORESTRUCTS_SOURCE_FILES}
    ${DISTORTIONCORRECTOR_SOURCE_FILES}
    ${FILEFORMATS_SOURCE_FILE}
    ${FLOWCOLORERS_SOURCE_FILES}
    ${FRAMESOURCES_SOURCE_FILES}
    ${RECTIFIER_SOURCE_FILES}
    ${SCRIPTING_SOURCE_FILE}
    ${SERIALIZER_SOURCE_FILES}
    ${STATISTICS_SOURCE_FILES}
    ${UIS_SOURCE_FILES}
    ${VISITORS_SOURCE_FILES}
    ${WIDGETS_SOURCE_FILES}
    ${IF_OPENGL_SOURCE_FILES}
    )

set(RESOURCES
    ../resources/main.qrc
    )

assign_source_group(${HEADERS} ${SOURCES} ${RESOURCES})

add_library(${PROJECT_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
    )

add_definitions(-DWITH_AVCODEC -DWITH_SWSCALE)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

set(ADDITIONAL_LIBRARIES)

if(WITH_OPENGL)
    find_package(Qt5 COMPONENTS REQUIRED OpenGL)
    set(ADDITIONAL_LIBRARIES
        ${ADDITIONAL_LIBRARIES}
        Qt5::OpenGL
        GLU
        GL
        )
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs 
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Script
    Qt5::Xml
    Qt5::SerialPort
    OPENCVwrapper
    JSONMODERNwrapper
    LIBPNGwrapper
    LIBJPEGwrapper
    V4L2wrapper
    AVCODECwrapper
    ${ADDITIONAL_LIBRARIES}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    AUTOMOC "TRUE"
    AUTOUIC "TRUE"
    AUTORCC "TRUE"
    FOLDER "${MODULE_NAME}"
    )