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

find_package(Qt5 COMPONENTS REQUIRED Core Xml)
target_link_libraries(${NAME} corecvs Qt5::Core Qt5::Xml )
