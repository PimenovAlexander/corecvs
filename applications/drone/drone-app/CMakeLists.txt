cmake_minimum_required(VERSION 3.11)
init_project(PROJECT_NAME drone-app)

find_package(Qt5 COMPONENTS REQUIRED Gui)

set(SOURCES_FILE
    mainDrone.cpp
    )

set(SOURCES
    ${SOURCES_FILE}
    )

add_executable(${PROJECT_NAME}
    ${SOURCES}
    )

target_link_libraries(${PROJECT_NAME}
    drone-ui
    drone-core
    corecvs_utils
    corecvs
    pthread
    Qt5::Gui
    OPENCVwrapper
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}/drone"
    )