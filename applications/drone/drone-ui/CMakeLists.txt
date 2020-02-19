cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications/drone)
init_project(PROJECT_NAME drone-ui)

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script)

set(PUBLIC_HEADER_FILES
    calibration/calibrationWidget.h
    calibration/imageForCalibrationWidget.h
    copterInputsWidget.h
    frameProcessor.h
    joystick/JoystickOptionsWidget.h
    joystick/mixerChannelOperationWidget.h
    physicsAboutWidget.h
    physicsMainWindow.h
    radio/radioControlWidget.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    )

set(SOURCE_FILES
    copterInputsWidget.cpp
    frameProcessor.cpp
    physicsAboutWidget.cpp
    physicsMainWindow.cpp
    )

set(CALIBRATION_SOURCE_FILES
    calibration/calibrationWidget.cpp
    calibration/imageForCalibrationWidget.cpp
    )

set(JOYSTICK_SOURCE_FILES
    joystick/JoystickOptionsWidget.cpp
    joystick/mixerChannelOperationWidget.cpp
    )

set(RADIO_SOURCE_FILE
    radio/radioControlWidget.cpp
    )

set(SOURCES
	${SOURCE_FILES}
    ${CALIBRATION_SOURCE_FILES}
    ${JOYSTICK_SOURCE_FILES}
    ${RADIO_SOURCE_FILE}
    )

set(RESOURCES
    drone.qrc
    )

assign_source_group(${HEADERS} ${SOURCES} ${RESOURCES})

add_library(${PROJECT_NAME} STATIC
	${HEADERS}
	${SOURCES}
    ${RESOURCES}
    )

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${AUTOGEN_BUILD_DIR}
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    corecvs_utils
    corecvs
    drone-core
    JOYSTICKwrapper
    Qt5::Core
    Qt5::Gui
    Qt5::Script
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