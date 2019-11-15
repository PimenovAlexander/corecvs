project (Adoptcolor)


set (FILELOADER_NAME adoptcolor )

set (SRC_FILES
    main_adoptcolor.cpp
)

if (PNG_LIB)
    message ("Adoptcolor would use LibPng")
    include(../../wrappers/libpng/sourcelist.cmake)    
endif()
    
if (JPEG_LIB)
    message ("Adoptcolor would use LibJpeg")
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

add_executable(${FILELOADER_NAME} ${SRC_FILES} ${HDR_FILES})


target_link_libraries(${FILELOADER_NAME} ${LIBS} stdc++fs corecvs)


