cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME grab24_qt)

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui)

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
    corecvs_utils
    pthread
    corecvs
	Qt5::Widgets
    Qt5::Gui
	V4L2wrapper
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    AUTOMOC TRUE
    AUTOUIC TRUE
    AUTORCC TRUE
    FOLDER "${MODULE_NAME}"
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )