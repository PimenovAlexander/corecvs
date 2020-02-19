cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME sdrRecord)

set(PRIVATE_HEADER_FILES
    sdrRecord.h
    )

set(HEADERS
    ${PRIVATE_HEADER_FILES}
    )

set(SOURCE_FILES
    sdrRecord.cpp 
    main.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    pthread
    corecvs
    SoapySDR
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )