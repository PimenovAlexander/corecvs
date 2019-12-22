project (Grab24)


set (NAME grab24 )

set (SRC_FILES
    main_grab24.cpp
)

add_executable(${NAME} ${SRC_FILES})

add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${NAME}  ${CMAKE_BINARY_DIR}/bin/${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${NAME} to binary directory"
        )

target_link_libraries(${NAME} cvs_utils corecvs pthread ${LIBS})
target_include_directories(${NAME} PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)

