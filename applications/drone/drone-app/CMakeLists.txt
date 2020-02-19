cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications/drone)
init_project(PROJECT_NAME drone-app)

set(SOURCE_FILE
    mainDrone.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${SOURCES})

add_executable(${PROJECT_NAME}
    ${SOURCES}
    )

set(ADDITIONAL_LIBS)

if(OpenCV_LIBS)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        OPENCVwrapper
        )
endif()

if(AVCODEC_LIBS)
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
    add_definitions(-DWITH_JPEG)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs
    corecvs_utils
    drone-ui
    pthread
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