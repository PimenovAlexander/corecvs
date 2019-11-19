project (PatternDetector)

set (NAME pattern_detector )

set (SRC_FILES
    main_pattern_detector.cpp      
)


add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)    
endif()
    
if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

include(../../wrappers/jsonmodern/sourcelist.cmake)

if (OpenCV_LIBS)
include(../../wrappers/opencv/sourcelist.cmake)
endif()

#if (apriltag_LIBS)
#    include(../../wrappers/apriltag_wrapper/sourcelist.cmake)
    find_package(Threads)
    target_link_libraries(${NAME} apriltag Threads::Threads)
#    target_include_directories(${NAME} PUBLIC APRILTAG_INCLUDE_DIR)
#endif()

message(STATUS PatternDetector:${INCLUDEPATHS} )


target_link_libraries(${NAME} corecvs pthread cvs_utils ${LIBS})

 
