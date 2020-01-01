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

if (AVCODEC_LIBS)
    message("Switching on avcodec support <${AVCODEC_LIBS}>")
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        AVCODECwrapper
        )
endif()

target_link_libraries(${PROJECT_NAME}
    stdc++fs
    corecvs
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )