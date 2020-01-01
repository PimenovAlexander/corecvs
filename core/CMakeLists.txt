CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
set(MODULE_NAME corecvs)
init_project(PROJECT_NAME ${MODULE_NAME})

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
#    placer
    delaunay
    joystick)

message(STATUS "Core will use ${CORE_SUBMODULES} and ${ADDITIONAL_MODULES}")

add_library(${PROJECT_NAME} SHARED)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ..
    )

set(ADDITIONAL_LIBS)

if (OpenBLAS_LIB)
    message("Core: Would use OpenBLAS adding <${OpenBLAS_LIB}> to dependancies")
    message("Core: Would use Lapacke  adding <${Lapacke_LIB}> to dependancies")
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        OPENBLAS::OPENBLAS
        LAPACKE::LAPACKE
        )   
endif()

if (TBB_LIBRARY)
    message("Core: Would use OpenBLAS adding <${OpenBLAS_LIB}> to dependancies")
    message("Core: Would use Lapacke  adding <${Lapacke_LIB}> to dependancies")
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        TBB::TBB
        )   
endif()

target_link_libraries(${PROJECT_NAME}
    pthread
    ${ADDITIONAL_LIBS}
    )

foreach(core_module ${CORE_SUBMODULES})
    message(STATUS "including ${core_module}")
    add_subdirectory(${core_module})
endforeach(core_module)

foreach(add_core_module ${ADDITIONAL_MODULES})
    message(STATUS "including ${add_core_module}")
    add_subdirectory(${add_core_module})
endforeach(add_core_module)