cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications)
init_project(PROJECT_NAME imageview)

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

set(PRIVATE_HEADER_FILE
    imageViewMainWindow.h
    )

set(HEADERS
    ${PRIVATE_HEADER_FILE}
    )

set(SOURCE_FILES
    imageViewMainWindow.cpp
    main_imageview.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs_utils
    corecvs
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    )

set_target_properties(${PROJECT_NAME}     
    PROPERTIES                             
    AUTOMOC TRUE
    AUTOUIC TRUE
    FOLDER "${MODULE_NAME}"
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )