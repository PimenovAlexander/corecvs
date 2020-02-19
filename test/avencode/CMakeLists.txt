cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME avencode)

set(SOURCE_FILE
    main_avencode.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${SOURCES})

add_executable(${PROJECT_NAME}
    ${SOURCE_FILE}
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

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    stdc++fs
    corecvs
    ${ADDITIONAL_LIBS}
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )