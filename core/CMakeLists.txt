CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

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
        #    placer
        delaunay
        joystick
    )

message(STATUS "Core will use ${CORE_SUBMODULES} and ${ADDITIONAL_MODULES}")

add_library(corecvs STATIC)

set_property(TARGET corecvs PROPERTY CXX_STANDARD 17)
set_property(TARGET corecvs PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(corecvs PUBLIC ..)

target_link_libraries(corecvs pthread)

if (OpenBLAS_LIB)
    message("Core: Would use OpenBLAS adding <${OpenBLAS_LIB}> to dependancies")
    message("Core: Would use Lapacke  adding <${Lapacke_LIB}> to dependancies")
    target_include_directories(corecvs PUBLIC ${OpenBLAS_INCLUDE_DIR} ${Lapacke_INCLUDE_DIR})
    target_link_libraries(corecvs ${OpenBLAS_LIB} ${Lapacke_LIB})
endif()

if (TBB_LIBRARY)
    message("Core: Would use TBB")
    include_directories (${TBB_INCLUDE_DIR})
    target_link_libraries(corecvs ${TBB_LIBRARY})
endif()

foreach(core_module ${CORE_SUBMODULES})
    message(STATUS "including ${core_module}")
    add_subdirectory(${core_module})
endforeach(core_module)

foreach(add_core_module ${ADDITIONAL_MODULES})
    message(STATUS "including ${add_core_module}")
    add_subdirectory(${add_core_module})
endforeach(add_core_module)


# Additional stuff mostly for IDE only

file(GLOB CUR_ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/xml/*.xml)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CUR_ADD_SRC_FILES})

file(GLOB ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/../.github/workflows/*.*)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CUR_ADD_SRC_FILES})

target_sources(corecvs PRIVATE ${ADD_SRC_FILES})
set_source_files_properties(${ADD_SRC_FILES} PROPERTIES EXTERNAL_OBJECT true HEADER_FILE_ONLY TRUE)

