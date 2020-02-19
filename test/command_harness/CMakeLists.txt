cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME command_harness)

set(SOURCE_FILE
    main_command_harness.cpp
    )

#set(EXTERNAL_SOURCE_FILES
#    ../widgets_test/main_widgets_test.cpp
#    ../widgets_test/testNativeWidget.cpp
#    )

set(SOURCES
    ${SOURCE_FILE}
#    ${EXTERNAL_SOURCE_FILES}
    )

add_executable(${PROJECT_NAME}
    ${SOURCES}
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
    pthread
    stdc++fs
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )