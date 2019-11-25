project (Serialize1)

set (NAME serialize1 )

set (SRC_FILES
    main_serialize1.cpp
)


include(../../wrappers/jsonmodern/sourcelist.cmake)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)    
endif()
    
if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

#message(STATUS SoftRender:${INCLUDEPATHS} )

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

target_link_libraries(${NAME} corecvs pthread ${LIBS})

 
