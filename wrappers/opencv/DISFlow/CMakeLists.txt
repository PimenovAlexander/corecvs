cmake_minimum_required (VERSION 2.8)
project (FastPatchOF)

set (HDR_FILES ${HDR_FILES} ${CMAKE_CURRENT_LIST_DIR}/DISFlow.h)
set (SRC_FILES ${SRC_FILES} ${CMAKE_CURRENT_LIST_DIR}/DISFlow.cpp)

include(../../wrappers/opencv/DISFlow/run_OF_INT/CMakeLists.txt)
include(../../wrappers/opencv/DISFlow/run_OF_RGB/CMakeLists.txt)