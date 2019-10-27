project (PatternDetector)

set (NAME pattern_detector )

set (SOURCES
    main_pattern_detector.cpp      
)


include(../../wrappers/jsonmodern/CMakeLists.txt)
include(../../wrappers/libpng/CMakeLists.txt)
include(../../wrappers/libjpeg/CMakeLists.txt)

message(STATUS PatternDetector:${INCLUDEPATHS} )

add_executable(${NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${NAME} corecvs pthread ${LIBS})

 
