CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(nester)

add_executable(nester main_nester.cpp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )

target_sources(nester
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/nester.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main_nester.cpp
        ${CMAKE_CURRENT_LIST_DIR}/nester.cpp
)

target_link_libraries(nester cvs_utils corecvs)
target_include_directories(nester PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)