project (drone-calibrator)


set (SRC_FILES
    main_calibrator.cpp
)

add_executable(drone-calibrator  ${SRC_FILES})

target_link_libraries(drone-calibrator drone-ui drone-core cvs_utils corecvs pthread)
target_include_directories(drone-calibrator PUBLIC ${drone-ui_SOURCE_DIR} ${drone-core_SOURCE_DIR} ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR})

