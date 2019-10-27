project (Fileloader)


set (FILELOADER_NAME fileloader )

set (FILELOADER_SOURCES
    main_fileloader.cpp
)


add_executable(${FILELOADER_NAME} ${FILELOADER_SOURCES})

target_link_libraries(${FILELOADER_NAME} stdc++fs corecvs)

