project (cloudview)

add_executable(cloudview  ${CMAKE_CURRENT_LIST_DIR}/main_cloudview.cpp)

target_include_directories(cloudview PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)
target_link_libraries(cloudview cvs_utils corecvs)
