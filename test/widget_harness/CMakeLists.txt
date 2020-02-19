cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME widget_harness)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(PRIVATE_HEADER_FILE
    changeReceiver.h
    )
    
set(HEADERS
    ${PRIVATE_HEADER_FILE}
    )

set(SOURCE_FILE
    main_widget_harness.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CURRENT_SOURCE_DIR}
    )

set(ADDITIONAL_LIBS)

if(PNG_LIB)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        LIBPNGwrapper
        )
    add_definitions(-DWITH_LIBPNG)
endif()

if(JPEG_LIB)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        LIBJPEGwrapper
        )
    add_definitions(-DWITH_LIBJPEG)
endif()

target_link_libraries(${PROJECT_NAME}
    corecvs
    corecvs_utils
    Qt5::Widgets
    stdc++fs 
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )