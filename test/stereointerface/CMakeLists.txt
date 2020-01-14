project (StereoInterface)


set (STEREO_NAME stereo )

set (SRC_FILES
    main_stereo.cpp
)

set (HDR_FILES
)

add_executable(${STEREO_NAME} ${SRC_FILES} ${HDR_FILES})

target_link_libraries(${STEREO_NAME} corecvs pthread)

