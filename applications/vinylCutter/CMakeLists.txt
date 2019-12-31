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

target_link_libraries(vinylCutter cvs_utils corecvs)
target_include_directories(vinylCutter PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)