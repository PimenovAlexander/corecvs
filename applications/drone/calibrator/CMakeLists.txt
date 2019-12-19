project (drone-calibrator)


set (SRC_FILES
    main_calibrator.cpp
)

add_executable(drone-calibrator  ${SRC_FILES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )

target_link_libraries(drone-calibrator drone-ui drone-core cvs_utils corecvs pthread)
target_include_directories(drone-calibrator PUBLIC ${drone-ui_SOURCE_DIR} ${drone-core_SOURCE_DIR} ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR})

