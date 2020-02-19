cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications/drone)
init_project(PROJECT_NAME physics-test)

find_package(Qt5 COMPONENTS REQUIRED Gui)

set(PRIVATE_HEADER_FILES
    dzhanibekovBolt.h
    testPhysicsObject.h
    )

set(HEADERS
    ${PRIVATE_HEADER_FILES}
    )

set(SOURCE_FILES
    dzhanibekovBolt.cpp
    main_physics_test.cpp
    sim_physics_test.cpp
    testPhysicsObject.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    )

set(ADDITIONAL_LIBS)

if(AVCODEC_LIBS)
    set(ADDITIONAL_LIBS
            ${ADDITIONAL_LIBS}
            AVCODECwrapper
            )
    add_definitions(-DWITH_AVCODEC -DWITH_SWSCALE)
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs
    corecvs_utils
    drone-core
    gtest
    gtest_main
    JSONMODERNwrapper
    Qt5::Gui
    pthread
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME} 
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )