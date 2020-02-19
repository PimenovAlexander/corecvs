cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME wrappers)
init_project(PROJECT_NAME OPENCVwrapper)

find_package(Qt5 COMPONENTS REQUIRED Widgets)

set (USE_PCAFLOW ON)

set(PUBLIC_HEADER_FILES
    featureDetectorCV.h
    KLTFlow.h
    openCvFileReader.h
    openCVTools.h
    semiGlobalBlockMatching.h
    openCvCheckerboardDetector.h
    #faceDetect/faceDetect.h
    patternDetect/openCVSquareDetector.h
    patternDetect/openCVCheckerBoardDetector.h
    )

set(GENERATED_HEADER_FILES
    generated/openCVKLTParameters.h
    generated/openCVBMParameters.h
    generated/openCVSGMParameters.h
    generated/openCVSquareDetectorParameters.h
    generated/openCVCheckerBoardDetectorParameters.h
    )

#Disflow                                                                       #redo by adding as library
set(DISFLOW_DIR ${CMAKE_CURRENT_LIST_DIR}/../../siblings/OF_DIS/)

if(EXISTS "${DISFLOW_DIR}/oflow.cpp")
    message("DISFLOW has been found at <${DISFLOW_DIR}>.")
    set(DISFLOW_FOUND ON)
else()
    set(DISFLOW_FOUND OFF)
endif()

if(DISFLOW_FOUND)
    add_definitions(-DWITH_DISFLOW)
    set (DISFLOW_HDR_FILES
        ${CMAKE_CURRENT_LIST_DIR}/DISFlow/DISFlow.h
        ${CMAKE_CURRENT_LIST_DIR}/generated/disFlowParameters.h
        )
endif()

if (USE_PCAFLOW)
    set (PCAFLOW_HEADER_FILES
        PCAFlow/PCAFlowProcessor.h
        generated/openCVPCAFlowParameters.h
        )
endif()

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    ${GENERATED_HEADER_FILES}
    ${DISFLOW_HDR_FILES}
    ${PCAFLOW_HEADER_FILES}
    )

set(SOURCE_FILES
    openCVTools.cpp
    openCvFileReader.cpp
    KLTFlow.cpp
    semiGlobalBlockMatching.cpp
    openCvCheckerboardDetector.cpp
    openCvImageRemapper.cpp
    #faceDetect/faceDetect.cpp
    patternDetect/openCVSquareDetector.cpp
    patternDetect/openCVCheckerBoardDetector.cpp
    )

set(GENERATED_SOURCE_FILES
    generated/openCVKLTParameters.cpp
    generated/openCVBMParameters.cpp
    generated/openCVSGMParameters.cpp
    generated/openCVSquareDetectorParameters.cpp
    generated/openCVCheckerBoardDetectorParameters.cpp
    )

if (DISFLOW_FOUND)
    set (DISFLOW_SRC_FILES
        ${CMAKE_CURRENT_LIST_DIR}/DISFlow/DISFlow.cpp
        ${CMAKE_CURRENT_LIST_DIR}/generated/disFlowParameters.cpp
        )
        
    set (DISFLOW_SOURCE_FILES
        ${DISFLOW_DIR}/oflow.cpp
        ${DISFLOW_DIR}/patch.cpp
        ${DISFLOW_DIR}/patchgrid.cpp
        ${DISFLOW_DIR}/refine_variational.cpp
        ${DISFLOW_DIR}/FDF1.0.1/image.c
        ${DISFLOW_DIR}/FDF1.0.1/opticalflow_aux.c
        ${DISFLOW_DIR}/FDF1.0.1/solver.c
        )
        
    set_source_files_properties(${DISFLOW_SOURCE_FILES} PROPERTIES COMPILE_DEFINITIONS "SELECTMODE=1; SELECTCHANNEL=3")
endif()

if (USE_PCAFLOW)
    set (PCAFLOW_SOURCE_FILES
        PCAFlow/PCAFlowProcessor.cpp
        generated/openCVPCAFlowParameters.cpp
        )
endif()

set(SOURCES
    ${SOURCE_FILES}
    ${GENERATED_SOURCE_FILES}
    ${DISFLOW_SRC_FILES}
    ${DISFLOW_SOURCE_FILES}
    ${PCAFLOW_SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_library(${PROJECT_NAME} STATIC
    ${HEADERS}
    ${SOURCES}
    )

set(ADDITIONAL_DIRS)

if (DISFLOW_FOUND)
    set(ADDITIONAL_DIRS
        ${ADDITIONAL_DIRS}
        ${EIGEN_INCLUDE_DIR}
        )
endif()    

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ADDITIONAL_DIRS}
    )

target_link_libraries(${PROJECT_NAME}
	PUBLIC
    ${OpenCV_LIBS}
	corecvs
    Qt5::Widgets
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    AUTOUIC TRUE
    FOLDER "${MODULE_NAME}"
    )