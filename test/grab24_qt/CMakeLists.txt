cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME Grab24_qt)

set(PRIVATE_HEADER_FILE
    main_grab24_qt.h
    )

set(HEADERS
    ${PRIVATE_HEADER_FILE}
    )

set(SOURCE_FILE
    main_grab24_qt.cpp
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
    cvs_utils
    pthread
    corecvs
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    AUTOMOC "TRUE"
    AUTOUIC "TRUE"
    AUTORCC "TRUE"
    FOLDER "${MODULE_NAME}"
    )