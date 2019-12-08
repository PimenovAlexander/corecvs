CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(imageview)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script)

add_executable(imageview main_imageview.cpp)

target_sources(imageview
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main_imageview.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.cpp
)

# Temporary fixes 
SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/imageview_autogen/include")
target_include_directories(imageview PUBLIC ${AUTOGEN_BUILD_DIR})

target_link_libraries(imageview cvs_utils corecvs)
target_include_directories(imageview PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)
