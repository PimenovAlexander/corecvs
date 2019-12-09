CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(cloudview)

add_executable(cloudview main_cloudview.cpp)

target_sources(cloudview
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main_cloudview.cpp
)

target_link_libraries(cloudview cvs_utils corecvs)
target_include_directories(cloudview PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)
