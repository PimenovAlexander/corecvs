project (Generator)

set (NAME generator)

set(SRC_FILES
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/pdoGenerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/widgetUIGenerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/documentationGenerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/baseGenerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapperGenerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/configLoader.cpp
)

set(HDR_FILES
        ${CMAKE_CURRENT_LIST_DIR}/reflectionGenerator.h
        ${CMAKE_CURRENT_LIST_DIR}/pdoGenerator.h
        ${CMAKE_CURRENT_LIST_DIR}/widgetUIGenerator.h
        ${CMAKE_CURRENT_LIST_DIR}/documentationGenerator.h
        ${CMAKE_CURRENT_LIST_DIR}/baseGenerator.h
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapperGenerator.h
        ${CMAKE_CURRENT_LIST_DIR}/configLoader.h
)

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${NAME}  ${CMAKE_BINARY_DIR}/bin/${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${NAME} to binary directory"
        )

find_package(Qt5 COMPONENTS REQUIRED Core Xml)
target_link_libraries(${NAME} corecvs Qt5::Core Qt5::Xml )


# Additional stuff mostly for IDE only
file(GLOB CUR_ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/xml/*.xml)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CUR_ADD_SRC_FILES} ${CMAKE_CURRENT_LIST_DIR}/../../wrappers/opencv/xml/*.xml)

file(GLOB ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.sh)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CUR_ADD_SRC_FILES})

target_sources(${NAME} PRIVATE ${ADD_SRC_FILES})
set_source_files_properties(${ADD_SRC_FILES} PROPERTIES EXTERNAL_OBJECT true HEADER_FILE_ONLY TRUE)
