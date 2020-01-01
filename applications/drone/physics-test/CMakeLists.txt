cmake_minimum_required(VERSION 3.11)
init_project(PROJECT_NAME drone-physics-test)

set(SOURCE_FILE
    main_physics_test.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

#if(AVCODEC_LIBS)
#    message("Switching on avcodec support.")
#    include(../../../wrappers/avcodec/sourcelist.cmake)
#endif()

assign_source_group(${SOURCES})

add_executable(${PROJECT_NAME}
    ${SOURCES}
    )

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

set (ADDITIONAL_LIBS)

if (AVCODEC_LIBS)
    set (ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        avcodec
        )
endif()

target_link_libraries(${PROJECT_NAME}
    gtest
    gtest_main
    drone-core
    cvs_utils
    corecvs
    pthread
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
    FOLDER "${MODULE_NAME}/drone"
    )