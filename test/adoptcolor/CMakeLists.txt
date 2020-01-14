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

add_custom_command(TARGET ${FILELOADER_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${FILELOADER_NAME}  ${CMAKE_BINARY_DIR}/bin/${FILELOADER_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${FILELOADER_NAME} to binary directory"
        )

include_directories(${INC_PATHS})


target_link_libraries(${FILELOADER_NAME} ${LIBS} stdc++fs corecvs)


