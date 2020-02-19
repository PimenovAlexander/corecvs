cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME wrappers)
init_project(PROJECT_NAME AVCODECwrapper)

set(PUBLIC_HEADER_FILES
    aviCapture.h
    rtspCapture.h
    avEncoder.h
    swScaler.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    )

set(SOURCE_FILES
    aviCapture.cpp
    rtspCapture.cpp
    avEncoder.cpp
    swScaler.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_library(${PROJECT_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
    )

add_definitions(-DWITH_AVCODEC -DWITH_SWSCALE)

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${AVCODEC_INCLUDES}
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    ${AVCODEC_LIBS}
    PRIVATE
    corecvs
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )