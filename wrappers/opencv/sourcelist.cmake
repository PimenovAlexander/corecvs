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

# Features Block
option(with_renderer "Should compile renderer" YES)

if(with_renderer)

endif()

include_directories(${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/patternDetect) 

    include_directories(PUBLIC ${OpenCV_INCLUDE_DIRS})
    link_libraries(${MODULE_NAME} ${OpenCV_LIBS})
