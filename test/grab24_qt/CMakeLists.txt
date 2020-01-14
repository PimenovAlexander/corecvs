project (Grab24_qt)


set (NAME grab24_qt )

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTORCC "YES")

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

set (SRC_FILES
    main_grab24_qt.cpp    
)

set (HDR_FILES
    main_grab24_qt.h
)

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${NAME}  ${CMAKE_BINARY_DIR}/bin/${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${NAME} to binary directory"
        )

target_link_libraries(${NAME} cvs_utils corecvs pthread ${LIBS})
target_include_directories(${NAME} PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR})

