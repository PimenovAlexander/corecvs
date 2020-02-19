cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME wrappers)
init_project(PROJECT_NAME LIBPNGwrapper)

set(PUBLIC_HEADER_FILE
    libpngFileReader.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILE}
    )

set(SOURCE_FILE
    libpngFileReader.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_library(${PROJECT_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
    )

add_definitions(-DWITH_LIBPNG)

target_include_directories(${PROJECT_NAME}
	PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    PNG::PNG
	PRIVATE
	corecvs
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )