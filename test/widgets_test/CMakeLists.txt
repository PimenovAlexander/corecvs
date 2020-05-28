cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME widgets_test)

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui)

set(PRIVATE_HEADER_FILES
    changeReceiver.h
    testNativeWidget.h
    )
    
set(HEADERS
    ${PRIVATE_HEADER_FILES}
    )

set(SOURCE_FILES
    main_widgets_test.cpp
    testNativeWidget.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CURRENT_SOURCE_DIR}
    )

set(ADDITIONAL_LIBS)

if (PNG_LIB)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        LIBPNGwrapper
        )
endif()

if (JPEG_LIB)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        LIBJPEGwrapper
        )
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs
    corecvs_utils
    Qt5::Widgets
    Qt5::Gui
    stdc++fs 
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    AUTOMOC TRUE
    AUTOUIC TRUE
    AUTORCC TRUE
    FOLDER "${MODULE_NAME}"
    )