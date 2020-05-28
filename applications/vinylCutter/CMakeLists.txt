cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications)
init_project(PROJECT_NAME vinylCutter)

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

set(PRIVATE_HEADER_FILES
    gcodeHandler.h
    mainWindow.h
    myGraphicsView.h
    gcodeToSceneInterpreter.h
    )

set(HEADERS
    ${PRIVATE_HEADER_FILES}
    )

set(SOURCE_FILES
    main_vinylCutter.cpp
    mainWindow.cpp
    myGraphicsView.cpp
    gcodeHandler.cpp
    gcodeToSceneInterpreter.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    )

target_include_directories(${PROJECT_NAME} 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

file(GLOB RESOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/*.*)

message("CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}")


# Resource files copying:
# we don't want to copy if we're building in the source dir
if(NOT ${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_BINARY_DIR})
    copy_files(${PROJECT_NAME}
        RESOURCE_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/resources/
        )
endif()

# files are only copied if a target depends on them
#add_custom_target(data-target ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/resources/background.png")

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs_utils
    corecvs
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    )

set_target_properties(${PROJECT_NAME}     
    PROPERTIES                             
    AUTOMOC TRUE
    AUTOUIC TRUE
    AUTORCC TRUE
    FOLDER "${MODULE_NAME}"
    )