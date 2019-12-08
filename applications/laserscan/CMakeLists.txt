CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(laserscan)

add_executable(laserscan main.cpp)

target_sources(laserscan
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_link_libraries(laserscan cvs_utils corecvs)
target_include_directories(laserscan PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)
