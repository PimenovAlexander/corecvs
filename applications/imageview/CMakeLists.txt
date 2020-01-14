project (imageview)

set(NAME imageview)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTORCC "YES")

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

set (UI_FILES
    ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.ui
    )

QT5_WRAP_UI( UI_HEADERS ${UI_FILES} )


add_executable(${NAME}
    ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.h

    ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/main_imageview.cpp
    ${UI_HEADERS}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )

SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_autogen/include")
target_include_directories(${NAME} PUBLIC ${AUTOGEN_BUILD_DIR} )
target_include_directories(${NAME} PUBLIC ${AUTOGEN_BUILD_DIR} ${CMAKE_CURRENT_BINARY_DIR})
message("AUTOGEN_BUILD_DIR bin directory <${AUTOGEN_BUILD_DIR}>")


target_include_directories(${NAME} PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)
target_link_libraries(${NAME} cvs_utils corecvs)

