cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME wrappers)
init_project(PROJECT_NAME V4L2wrapper)

find_package(Qt5 COMPONENTS REQUIRED Core) #Gui Widgets Script Xml SerialPort)

message(STATUS "Including V4L2Wrapper")

set(PUBLIC_HEADER_FILES
    V4L2CaptureDecouple.h  
    V4L2Capture.h 
    V4L2.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    )

set (SOURCE_FILES
    V4L2CaptureDecouple.cpp 
    V4L2Capture.cpp
    V4L2.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
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
    corecvs
    v4l2
    Qt5::Core
    )

add_definitions( -DWITH_LIBJPEG )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )