project (PatternDetector)

set (NAME pattern_detector )

set (SRC_FILES
    main_pattern_detector.cpp      
)


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

#message(STATUS PatternDetector:${INCLUDEPATHS} )

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

target_link_libraries(${NAME} corecvs pthread ${LIBS})

 
