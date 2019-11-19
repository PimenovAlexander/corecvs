project (Grab24_qt)


set (NAME grab24_qt )

set (SRC_FILES
    main_grab24_qt.cpp    
)

set (HDR_FILES
    main_grab24_qt.h
)


add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")
set(CMAKE_AUTORCC "YES")

target_link_libraries(${NAME} cvs_utils corecvs pthread ${LIBS})
target_include_directories(${NAME} PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR})

