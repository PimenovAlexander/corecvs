cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME wrappers)
init_project(PROJECT_NAME JSONMODERNwrapper)

set(PUBLIC_HEADER_FILES
    jsonModernReader.h
    sources/src/json.hpp
    )

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    )

set(SOURCE_FILE
    jsonModernReader.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_library(${PROJECT_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
    )

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_link_libraries(${PROJECT_NAME}
	PRIVATE
	corecvs
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )