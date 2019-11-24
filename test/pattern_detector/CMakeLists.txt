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

if (USE_APRILTAG)
    target_link_libraries(${NAME} ${APRILTAG_LIB} Threads::Threads)
endif()

message(STATUS PatternDetector:${INCLUDEPATHS} )


target_link_libraries(${NAME} corecvs pthread cvs_utils ${LIBS})

 
