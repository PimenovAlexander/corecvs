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

if (WITH_APRILTAG)
    MESSAGE(STATUS "Including Apriltag on pattern_detector build")
    find_package(apriltag REQUIRED)
    find_package(Threads REQUIRED)
    add_definitions(-DWITH_APRILTAG)
    target_link_libraries(${NAME} apriltag Threads::Threads)
endif()

message(STATUS PatternDetector:${INCLUDEPATHS} )


target_link_libraries(${NAME} corecvs pthread cvs_utils ${LIBS})

 
