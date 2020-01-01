set(SUBDIRECTORIES
    avcodec
    joystick
    jsonmodern
    libjpeg
    libpng
    opencv
    v4l2
    )

foreach(subdirectory ${SUBDIRECTORIES})
    message(STATUS "adding subdirectory/${subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${subdirectory})
endforeach(subdirectory)