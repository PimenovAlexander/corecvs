project (Grab24_qt)


set (NAME grab24_qt )

set (SRC_FILES
    main_grab24_qt.cpp
)

add_executable(${NAME} ${SRC_FILES})

target_link_libraries(${NAME} cvs_utils corecvs pthread ${LIBS})
target_include_directories(${NAME} PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)

