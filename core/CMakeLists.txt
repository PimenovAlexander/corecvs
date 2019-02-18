cmake_minimum_required (VERSION 3.1)
project (CoreCVSLib)

set (CORE_SUBMODULES
    alignment
    assignment
    automotive
    boosting
    buffers
    cammodel
    fileformats
    filesystem
    framesources
    filters
    function
    geometry
    kalman
    kltflow
    math
    meta
    meanshift
    rectification
    reflection
    segmentation
    stats
    tbbwrapper
    utils
    clustering3d
    patterndetection
    cameracalibration
    polynomial
    camerafixture
    iterative
    stereointerface
    tinyxml2 )

set ( ADDITIONAL_MODULES
    assignment
    boosting
    meta
    framesources
    filters
    meanshift
    segmentation
    clustering3d
    placer
    delaunay)

message(STATUS "Core will use ${CORE_SUBMODULES} and ${ADDITIONAL_MODULES}")

add_library(corecvs STATIC)

target_include_directories(corecvs PUBLIC ..)

foreach(core_module ${CORE_SUBMODULES})
    message(STATUS "including ${core_module}")
    add_subdirectory(${core_module})
endforeach(core_module)


