CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(vinylCutter)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script)

add_executable(vinylCutter main_vinylCutter.cpp)

target_sources(vinylCutter
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/mainWindow.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main_vinylCutter.cpp
        ${CMAKE_CURRENT_LIST_DIR}/mainWindow.cpp
        ${CMAKE_CURRENT_LIST_DIR}/myGraphicsView.cpp
        ${CMAKE_CURRENT_LIST_DIR}/gcodeHandler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/gcodeToSceneInterpreter.cpp
)

# Resource files copying:
# we don't want to copy if we're building in the source dir
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)

    # list of files for which we add a copy rule
    set(data_SHADOW resources/background.png)

    foreach(item IN LISTS data_SHADOW)
        message(STATUS ${item})
        add_custom_command(
                OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
                COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
                DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
        )
    endforeach()
endif()

# files are only copied if a target depends on them
add_custom_target(data-target ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/resources/background.png")

target_link_libraries(vinylCutter cvs_utils corecvs)
target_include_directories(vinylCutter PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)