cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME flow_detector)

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

if(AVCODEC_LIBS)
    message("Switching on avcodec support <${AVCODEC_LIBS}>")
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        AVCODECwrapper
        )
     add_definitions(-DWITH_AVCODEC -DWITH_SWSCALE)
endif()

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

if (OpenCV_LIBS)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        OPENCVwrapper
        )
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
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

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )