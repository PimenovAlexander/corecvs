cmake_minimum_required(VERSION 3.11)
init_project(PROJECT_NAME drone-ui)

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script)

set(PUBLIC_HEADER_FILES
    calibration/calibrationWidget.h
    calibration/imageForCalibrationWidget.h  #private may be
    copterInputsWidget.h #private may be
    frameProcessor.h
    joystick/JoystickOptionsWidget.h
    joystick/mixerChannelOperationWidget.h  #private may be
    physicsAboutWidget.h #private may be
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

assign_source_group(${HEADERS} ${SOURCES})

add_library(${PROJECT_NAME} STATIC
	${HEADERS}
	${SOURCES}
    )

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

set (ADDITIONAL_LIBS)

if (OpenCV_LIBS)
    set (ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        ${OpenCV_LIBS}
        )
endif()

target_link_libraries(${PROJECT_NAME}
    drone-core
    corecvs_utils
    corecvs
    ${ADDITIONAL_LIBS}
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Script
    JOYSTICKwrapper
    )



set_target_properties(${PROJECT_NAME}     
    PROPERTIES                             
    AUTOMOC TRUE
    AUTOUIC TRUE
    AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../../utils/uis
    FOLDER "${MODULE_NAME}/drone"
    )