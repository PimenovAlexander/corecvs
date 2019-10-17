project (Grab24)


set (NAME grab24 )

set (SOURCES
    main_grab24.cpp
)

add_executable(${NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${NAME} utils corecvs pthread ${LIBS})
target_include_directories(${NAME} PUBLIC ${corecvs_SOURCE_DIR} ${utils_SOURCE_DIR} .)

