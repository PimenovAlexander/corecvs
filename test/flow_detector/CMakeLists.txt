cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME FlowDetector)

set(SOURCE_FILE
    main_flow_detector.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${SOURCES})

add_executable(${PROJECT_NAME}
    ${SOURCES}
    )

set(ADDITIONAL_LIBS)

if (PNG_LIB)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        PNGwrapper
        )
endif()

if (JPEG_LIB)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        LIBJPEGwrapper
        )
endif()

if (OpenCV_LIBS)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        OPENCVwrapper
        )
endif()

target_link_libraries(${PROJECT_NAME}
    stdc++fs 
    corecvs
    pthread
    JSONMODERNwrapper
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )

#set (SOURCES ${SOURCES} ${JSON_MODERN_HEADERS} ${JSON_MODERN_SOURCES})

#add_definitions( -DWITH_JSONMODERN )
#include_directories(${JSON_MODERN_INCLUDES})
#message(STATUS FlowDetector:${JSON_MODERN_INCLUDES} )

#message(STATUS FlowDetector:${INCLUDEPATHS} )