cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications)
init_project(PROJECT_NAME cloudview)

set(SOURCE_FILE
    main_cloudview.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

assign_source_group(${SOURCES})

add_executable(${PROJECT_NAME}
    ${SOURCES}
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs
    corecvs_utils
    )

set_target_properties(${PROJECT_NAME}     
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )