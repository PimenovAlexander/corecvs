project(apriltag_test)

set(SRC_FILES
        ${CMAKE_CURRENT_LIST_DIR}/apriltag_test.cpp)
set(HDR_FILES
        ${CMAKE_CURRENT_LIST_DIR}/apriltag_test.h)

add_executable(apriltagtest ${SRC_FILES} ${HDR_FILES})

target_link_libraries(apriltagtest cvs_utils corecvs ${OpenCV_LIBS})