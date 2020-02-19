cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME flowtest)

set(SOURCE_FILE
    main_flowtest.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${SOURCES})

add_executable(${PROJECT_NAME}
    ${SOURCES}
    )

set(ADDITIONAL_LIBS)

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