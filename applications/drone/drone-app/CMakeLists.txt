project (drone-app)


set (SRC_FILES
    mainDrone.cpp
)

add_executable(drone-app  mainDrone.cpp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )


target_link_libraries(drone-app  drone-ui drone-core cvs_utils corecvs pthread event)
target_include_directories(drone-app PUBLIC ${drone-ui_SOURCE_DIR} ${drone-core_SOURCE_DIR} ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR})

