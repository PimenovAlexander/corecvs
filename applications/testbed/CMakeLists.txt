CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project (testbed)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_LIST_DIR}/ui" ${CMAKE_AUTOUIC_SEARCH_PATHS})

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script)

add_executable(testbed main_testbed.cpp)

target_sources(testbed
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/testbedMainWindow.h
        ${CMAKE_CURRENT_LIST_DIR}/pointScene.h
        ${CMAKE_CURRENT_LIST_DIR}/localHistogram.h
        ${CMAKE_CURRENT_LIST_DIR}/widgets/testbedImageWidget.h
        ${CMAKE_CURRENT_LIST_DIR}/gapFiller.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/testbedMainWindow.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main_testbed.cpp
        ${CMAKE_CURRENT_LIST_DIR}/pointScene.cpp
        ${CMAKE_CURRENT_LIST_DIR}/localHistogram.cpp
        ${CMAKE_CURRENT_LIST_DIR}/widgets/testbedImageWidget.cpp
)

# Temporary fixes 
SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/testbed_autogen/include")
target_include_directories(testbed PUBLIC ${AUTOGEN_BUILD_DIR})

target_include_directories(testbed PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)
target_link_libraries(testbed cvs_utils corecvs)
