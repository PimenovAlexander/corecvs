#project (OpenCVJSONWrapper)

message(STATUS "Including OpenCVWrapper")

set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/featureDetectorCV.h
    ${CMAKE_CURRENT_LIST_DIR}/KLTFlow.h
    ${CMAKE_CURRENT_LIST_DIR}/openCvFileReader.h
    ${CMAKE_CURRENT_LIST_DIR}/openCVTools.h
    ${CMAKE_CURRENT_LIST_DIR}/semiGlobalBlockMatching.h
    ${CMAKE_CURRENT_LIST_DIR}/openCvCheckerboardDetector.h
    ${CMAKE_CURRENT_LIST_DIR}/patternDetect/openCVSquareDetector.h
    ${CMAKE_CURRENT_LIST_DIR}/patternDetect/openCVCheckerBoardDetector.h

)

set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/openCVTools.cpp
    ${CMAKE_CURRENT_LIST_DIR}/openCvFileReader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/KLTFlow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/semiGlobalBlockMatching.cpp
    ${CMAKE_CURRENT_LIST_DIR}/openCvCheckerboardDetector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/openCvImageRemapper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/patternDetect/openCVSquareDetector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/patternDetect/openCVCheckerBoardDetector.cpp

)


# Generated block

set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVKLTParameters.h
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVBMParameters.h
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVSGMParameters.h
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVSquareDetectorParameters.h
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVCheckerBoardDetectorParameters.h

)

set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVKLTParameters.cpp
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVBMParameters.cpp
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVSGMParameters.cpp
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVSquareDetectorParameters.cpp
    ${CMAKE_CURRENT_LIST_DIR}/generated/openCVCheckerBoardDetectorParameters.cpp
)

# Disflow
set (DISFLOW_DIR ${CMAKE_CURRENT_LIST_DIR}/../../siblings/OF_DIS/)

if(EXISTS "${DISFLOW_DIR}/oflow.cpp")
  message("DISFLOW has been found at <${DISFLOW_DIR}>.")
  SET(DISFLOW_FOUND ON)
else()
  SET(DISFLOW_FOUND OFF)
endif()


if (DISFLOW_FOUND)
    add_definitions(-DWITH_DISFLOW)

    set (HDR_FILES
        ${HDR_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/DISFlow/DISFlow.h
        ${CMAKE_CURRENT_LIST_DIR}/generated/disFlowParameters.h
    )

    set (SRC_FILES
        ${SRC_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/DISFlow/DISFlow.cpp
        ${CMAKE_CURRENT_LIST_DIR}/generated/disFlowParameters.cpp
        )

    set (DISFLOW_SRC_FILES
        ${DISFLOW_DIR}/oflow.cpp
        ${DISFLOW_DIR}/patch.cpp
        ${DISFLOW_DIR}/patchgrid.cpp
        ${DISFLOW_DIR}/refine_variational.cpp
        ${DISFLOW_DIR}/FDF1.0.1/image.c
        ${DISFLOW_DIR}/FDF1.0.1/opticalflow_aux.c
        ${DISFLOW_DIR}/FDF1.0.1/solver.c
    )

    include_directories(PUBLIC ${EIGEN_INCLUDE_DIR})
    set_source_files_properties(${DISFLOW_SRC_FILES} PROPERTIES COMPILE_DEFINITIONS "SELECTMODE=1; SELECTCHANNEL=3")

    set (SRC_FILES
        ${SRC_FILES}
        ${DISFLOW_SRC_FILES}
    )
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/patternDetect)
include_directories(PUBLIC ${OpenCV_INCLUDE_DIRS})
link_libraries(${MODULE_NAME} ${OpenCV_LIBS})

