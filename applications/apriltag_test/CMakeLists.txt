cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications)
init_project(PROJECT_NAME apriltag_test)

set(PRIVATE_HEADER_FILE
    apriltag_test.h
    )

set(HEADERS
    ${PRIVATE_HEADER_FILE}
    )

set(SOURCE_FILE
    apriltag_test.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    )

target_link_libraries(${PROJECT_NAME}
    corecvs_utils
    corecvs
    APRILTAGwrapper
    ${OPENCV_LIBS}
    )

set_target_properties(${PROJECT_NAME}     
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )