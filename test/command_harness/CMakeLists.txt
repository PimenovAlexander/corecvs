project (command_harness)


set (NAME command_harness)

set (SRC_FILES
    main_command_harness.cpp
)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()
    
if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )

include_directories(${INC_PATHS})

target_link_libraries(${NAME} ${LIBS} cvs_utils stdc++fs corecvs)

