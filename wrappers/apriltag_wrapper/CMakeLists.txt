cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME wrappers)
init_project(PROJECT_NAME APRILTAGwrapper)

set(PUBLIC_HEADER_FILES
    apriltagDetector.h
    generated/apriltagParameters.h
    #generated/aprilTagType.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    )

set(SOURCE_FILES
    apriltagDetector.cpp
    generated/apriltagParameters.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

set(XML_FILES
    xml/apriltag.xml
    )

set(TOOLS_GENERATOR_FILE
    ${CMAKE_CURRENT_LIST_DIR}/../../tools/generator/regen-apriltag.sh
    )

set(RESOURCES
    ${XML_FILES}
    ${TOOLS_GENERATOR_FILE}
    )

assign_source_group(${HEADERS} ${SOURCES} ${RESOURCES})

add_library(${PROJECT_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
    )

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    APRILTAG::APRILTAG
    PRIVATE
    corecvs
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )