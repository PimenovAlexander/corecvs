set(SUBDIRECTORIES
    apriltag_wrapper
    avcodec
    #cblasLapack
    #cgal
    #directShow
    #eigen
    #gtest
    #gts
    joystick
    jsonmodern
    #libfftw
    libjpeg
    libpng
    opencv
    #pcl
    #rapidjson
    v4l2
    )

foreach(subdirectory ${SUBDIRECTORIES})
    message(STATUS "adding subdirectory/${subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${subdirectory})
endforeach(subdirectory)