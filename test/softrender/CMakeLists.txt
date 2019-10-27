project (SoftRender)

set (NAME softrender )

set (SOURCES
    main_softrender.cpp      
)


include(../../wrappers/jsonmodern/CMakeLists.txt)
include(../../wrappers/libpng/CMakeLists.txt)
include(../../wrappers/libjpeg/CMakeLists.txt)

message(STATUS SoftRender:${INCLUDEPATHS} )

add_executable(${NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${NAME} corecvs pthread ${LIBS})

 
