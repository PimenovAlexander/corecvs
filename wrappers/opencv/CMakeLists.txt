cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME wrappers)
init_project(PROJECT_NAME OPENCVwrapper)

set(PUBLIC_HEADER_FILES
    featureDetectorCV.h
    KLTFlow.h
    openCvFileReader.h
    openCVTools.h
    semiGlobalBlockMatching.h
    openCvCheckerboardDetector.h
    patternDetect/openCVSquareDetector.h
    )

set(GENERATED_HEADER_FILES
    generated/openCVKLTParameters.h
    generated/openCVBMParameters.h
    generated/openCVSGMParameters.h
    generated/openCVSquareDetectorParameters.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    ${GENERATED_HEADER_FILES}
    )

set(SOURCE_FILES
    openCVTools.cpp
    openCvFileReader.cpp
    KLTFlow.cpp
    semiGlobalBlockMatching.cpp
    openCvCheckerboardDetector.cpp
    openCvImageRemapper.cpp
    patternDetect/openCVSquareDetector.cpp
    )

set(GENERATED_SOURCE_FILES
    generated/openCVKLTParameters.cpp
    generated/openCVBMParameters.cpp
    generated/openCVSGMParameters.cpp
    generated/openCVSquareDetectorParameters.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    ${GENERATED_SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

# Features Block
option(with_renderer "Should compile renderer" YES)

add_library(${PROJECT_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_link_libraries(${PROJECT_NAME}
	PUBLIC
    ${OpenCV_LIBS}
	PRIVATE
	corecvs
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )