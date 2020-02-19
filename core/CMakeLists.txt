cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME corecvs)
init_project(PROJECT_NAME ${MODULE_NAME})

message(STATUS "Core will use ${CORE_SUBMODULES} and ${ADDITIONAL_MODULES}")

set(CORE_SUBMODULES
    alignment
    assignment
    automotive
    boosting
    buffers
    cameracalibration
    camerafixture
    cammodel
    clustering3d
    fileformats
    filesystem
    filters
    framesources
    function
    geometry
    iterative
    kalman
    kltflow
    math
    meanshift
    meta
    patterndetection
    polynomial
    rectification
    reflection
    segmentation
    stats
    stereointerface
    tbbwrapper
    tinyxml2
    utils
    xml/generated
    )

set(ADDITIONAL_MODULES
    #placer
    delaunay
    joystick
    )

foreach(core_module ${CORE_SUBMODULES})
    message(STATUS "including ${core_module}")
    add_subdirectory(${core_module})
    string(TOUPPER ${core_module} core_module)
    set(HEADERS
        ${HEADERS}
        ${${core_module}_HEADER_FILES}
        )
    set(SOURCES
        ${SOURCES}
        ${${core_module}_SOURCE_FILES}
        )
endforeach(core_module)

foreach(add_core_module ${ADDITIONAL_MODULES})
    message(STATUS "including ${add_core_module}")
    add_subdirectory(${add_core_module})
    string(TOUPPER ${add_core_module} add_core_module)
    set(HEADERS
        ${HEADERS}
        ${${add_core_module}_HEADER_FILES}
        )
    set(SOURCES
        ${SOURCES}
        ${${add_core_module}_SOURCE_FILES}
        )
endforeach(add_core_module)

set(XML_RESOURCE_FILES
    xml/basemock.xml
    xml/clustering1.xml
    xml/precise.xml
    xml/bufferFilters.xml
    xml/distortion.xml
    xml/parameters.xml
    xml/projections.xml
    xml/calibration.xml
    xml/filterBlock.xml
    xml/patternDetector.xml
    xml/stereoAlign.xml
    )

set(RESOURCES
    ${XML_RESOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/../.github/workflows/ccpp.yaml
    ${CMAKE_CURRENT_LIST_DIR}/../.github/workflows/ubuntu_no_opencv.yml
    )

set_source_files_properties(${RESOURCES} 
    PROPERTIES 
    EXTERNAL_OBJECT TRUE 
    HEADER_FILE_ONLY TRUE
    )

assign_source_group(${HEADERS} ${SOURCES} ${RESOURCES})

add_library(${PROJECT_NAME} STATIC
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
    )

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ..
    )

set(ADDITIONAL_LIBS)

if(OpenBLAS_LIB)
    message("Core: Would use OpenBLAS adding <${OpenBLAS_LIB}> to dependancies")
    message("Core: Would use Lapacke  adding <${Lapacke_LIB}> to dependancies")
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        OPENBLAS::OPENBLAS
        LAPACKE::LAPACKE
        )   
endif()

if(TBB_LIBRARY)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        TBB::TBB
        )   
endif()

target_link_libraries(${PROJECT_NAME}
    pthread
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )