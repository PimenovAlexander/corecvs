project (drone-app)


set (SRC_FILES
    mainDrone.cpp
)

add_executable(drone-app  mainDrone.cpp)

target_link_libraries(drone-app  drone-ui drone-core cvs_utils corecvs pthread)
target_include_directories(drone-app PUBLIC ${drone-ui_SOURCE_DIR} ${drone-core_SOURCE_DIR} ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)

