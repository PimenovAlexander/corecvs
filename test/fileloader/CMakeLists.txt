project (Fileloader)


set (FILELOADER_NAME fileloader )

set (SRC_FILES
    main_fileloader.cpp
)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()
    
if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

add_executable(${FILELOADER_NAME} ${SRC_FILES} ${HDR_FILES})

target_link_libraries(${FILELOADER_NAME} stdc++fs corecvs)

