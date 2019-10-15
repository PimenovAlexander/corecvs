project (corecvs)

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
    tinyxml2
    xml/generated 
)

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


if (OpenBLAS_LIB)
    include_directories (${OpenBLAS_INCLUDE_DIR})
    add_definitions(-DWITH_BLAS)
endif()

if (TBB_LIB)
    include_directories (${TBB_INCLUDE_DIR})
    add_definitions(-DWITH_TBB)
endif()

# include_directories(corecvs PUBLIC ..)

add_library(corecvs STATIC )




set_property(TARGET corecvs PROPERTY CXX_STANDARD 17)
set_property(TARGET corecvs PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(corecvs PUBLIC ..)

target_link_libraries(corecvs stdc++fs)


foreach(core_module ${CORE_SUBMODULES})
    message(STATUS "including ${core_module}")
    add_subdirectory(${core_module})
endforeach(core_module)

#install(TARGETS corecvs
#  EXPORT FooBarTargets
#  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
#  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/core"
#  )



