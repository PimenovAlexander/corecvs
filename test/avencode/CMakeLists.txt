project (avencode)

set (MODULE_NAME avencode)

set (SRC_FILES
    main_avencode.cpp
)


if(AVCODEC_LIBS) 
    message("Switching on avcodec support <${AVCODEC_LIBS}>")    
    include(../../wrappers/avcodec/sourcelist.cmake)
endif()

if(GIF_FOUND)
    message("Switching on gif support <${GIF_LIB}>")
    include(../../wrappers/libgif/sourcelist.cmake)
endif()

add_executable(${MODULE_NAME} ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )

include_directories(${INC_PATHS})

target_link_libraries(${MODULE_NAME} ${LIBS} stdc++fs corecvs  )

