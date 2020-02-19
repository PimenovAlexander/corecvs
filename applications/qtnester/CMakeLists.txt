cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications)
init_project(PROJECT_NAME qtnester)

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

set(PRIVATE_HEADER_FILES
    imageViewMainWindow.h
    )

set(HEADERS
    ${PRIVATE_HEADER_FILES}
    )

set(SOURCE_FILES
    imageViewMainWindow.cpp
    main_qtnester.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME}
	${HEADERS}
	${SOURCES}
    )

#SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_autogen/include")
#target_include_directories(${NAME} PUBLIC ${AUTOGEN_BUILD_DIR} )
#target_include_directories(${NAME} PUBLIC ${AUTOGEN_BUILD_DIR} ${CMAKE_CURRENT_BINARY_DIR})
#message("AUTOGEN_BUILD_DIR bin directory <${AUTOGEN_BUILD_DIR}>")

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${AUTOGEN_BUILD_DIR}
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs_utils
    corecvs
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    ${OpenCV_LIBS}
    )

set_target_properties(${PROJECT_NAME}     
    PROPERTIES                             
    AUTOMOC TRUE
    AUTOUIC TRUE
    AUTORCC TRUE
    FOLDER "${MODULE_NAME}"
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )