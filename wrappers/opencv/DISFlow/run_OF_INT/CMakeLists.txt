cmake_minimum_required (VERSION 2.8)

FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

set (HDR_FILES ${HDR_FILES} ${CMAKE_CURRENT_LIST_DIR}/runOF_RGB.h)

set(CODEFILES
        /siblings/OF_DIS/oflow.cpp
        /siblings/OF_DIS/patch.cpp
        /siblings/OF_DIS/patchgrid.cpp
        /siblings/OF_DIS/refine_variational.cpp
        /siblings/OF_DIS/FDF1.0.1/image.c
        /siblings/OF_DIS/FDF1.0.1/opticalflow_aux.c
        /siblings/OF_DIS/FDF1.0.1/solver.c)

# RGB, Optical Flow
add_library (run_OF_RGB ${CODEFILES})
set_target_properties (run_OF_RGB PROPERTIES COMPILE_DEFINITIONS "SELECTMODE=1")
set_property(TARGET run_OF_RGB APPEND PROPERTY COMPILE_DEFINITIONS "SELECTCHANNEL=3") # use RGB image
TARGET_LINK_LIBRARIES(run_OF_RGB ${OpenCV_LIBS})