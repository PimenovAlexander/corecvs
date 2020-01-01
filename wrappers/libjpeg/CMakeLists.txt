cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME wrappers)
init_project(PROJECT_NAME LIBJPEGwrapper)

set(PUBLIC_HEADER_FILE
    libjpegFileReader.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILE}
    )

set(SOURCE_FILE
    libjpegFileReader.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_library(${PROJECT_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
    )

add_definitions(-DWITH_LIBJPEG)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    JPEG::JPEG
	PRIVATE
	corecvs
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )