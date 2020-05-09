cmake_minimum_required(VERSION 3.5)

project(vkwidgettest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(vkwidgettest
    vkwidgettest.cpp
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Copying ${PROJECT_NAME} to binary directory"
)

set(WITH_VULKAN_QT_IB ON CACHE BOOL "" FORCE)

target_link_libraries(vkwidgettest PRIVATE cvs_utils corecvs)

