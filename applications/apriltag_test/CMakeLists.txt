project(apriltag_test)

set(SRC_FILES
        ${CMAKE_CURRENT_LIST_DIR}/apriltag_test.cpp)
set(HDR_FILES
        ${CMAKE_CURRENT_LIST_DIR}/apriltag_test.h)

add_executable(apriltag_test ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET <kernel-target> POST_BUILD
    COMMAND cp ${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Copying ${PROJECT_NAME} to binary directory"
)
#file(COPY ${PROJECT_NAME} DESTINATION ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(apriltagtest cvs_utils corecvs ${OpenCV_LIBS})