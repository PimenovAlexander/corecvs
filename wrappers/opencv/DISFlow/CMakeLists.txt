#project (FastPatchOF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wno-unknown-pragmas -Wall -std=c++11 -msse4")  #-Wall
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wno-unknown-pragmas -Wall -msse4")  #-Wall

FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

set(CODEFILES
        ${CMAKE_SOURCE_DIR}/siblings/OF_DIS/oflow.cpp
        ${CMAKE_SOURCE_DIR}/siblings/OF_DIS/patch.cpp
        ${CMAKE_SOURCE_DIR}/siblings/OF_DIS/patchgrid.cpp
        ${CMAKE_SOURCE_DIR}/siblings/OF_DIS/refine_variational.cpp
        ${CMAKE_SOURCE_DIR}/siblings/OF_DIS/FDF1.0.1/image.c
        ${CMAKE_SOURCE_DIR}/siblings/OF_DIS/FDF1.0.1/opticalflow_aux.c
        ${CMAKE_SOURCE_DIR}/siblings/OF_DIS/FDF1.0.1/solver.c)

# RGB, Optical Flow
add_library (OF_RGB ${CODEFILES})
set_target_properties (OF_RGB PROPERTIES COMPILE_DEFINITIONS "SELECTMODE=1")
set_property(TARGET OF_RGB APPEND PROPERTY COMPILE_DEFINITIONS "SELECTCHANNEL=3") # use RGB image
TARGET_LINK_LIBRARIES(OF_RGB ${OpenCV_LIBS})


set (HDR_FILES ${HDR_FILES} ${CMAKE_SOURCE_DIR}/wrappers/opencv/DISFlow/DISFlow.h)
set (SRC_FILES ${SRC_FILES} ${CMAKE_SOURCE_DIR}/wrappers/opencv/DISFlow/DISFlow.cpp)