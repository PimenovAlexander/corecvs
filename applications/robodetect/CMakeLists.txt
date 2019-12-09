CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(robodetect)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_LIST_DIR}/ui" ${CMAKE_AUTOUIC_SEARCH_PATHS})

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script)

add_executable(robodetect main_robodetect.cpp)

target_sources(robodetect
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/robodetectMainWindow.h
        ${CMAKE_CURRENT_LIST_DIR}/pointScene.h
        ${CMAKE_CURRENT_LIST_DIR}/localHistogram.h
        ${CMAKE_CURRENT_LIST_DIR}/widgets/robodetectImageWidget.h
        ${CMAKE_CURRENT_LIST_DIR}/gapFiller.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/robodetectMainWindow.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main_robodetect.cpp
        ${CMAKE_CURRENT_LIST_DIR}/pointScene.cpp
        ${CMAKE_CURRENT_LIST_DIR}/localHistogram.cpp
        ${CMAKE_CURRENT_LIST_DIR}/widgets/robodetectImageWidget.cpp
)

SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/robodetect_autogen/include")
target_include_directories(robodetect PUBLIC ${AUTOGEN_BUILD_DIR} )

target_link_libraries(robodetect cvs_utils corecvs)
target_include_directories(robodetect PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)