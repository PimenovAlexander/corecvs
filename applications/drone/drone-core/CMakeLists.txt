cmake_minimum_required(VERSION 3.11)
init_project(PROJECT_NAME drone-core)

find_package(Qt5 COMPONENTS REQUIRED SerialPort Core Gui)

set(PUBLIC_HEADER_FILES
    clientSender.h
    joystickInput.h
    comcontroller.h
    controlRecord.h
    copterInputs.h
    autopilot/protoautopilot.h
    autopilot/vertexsquare.h
    calibration/calibration.h
    copter/motor.h
    copter/quad.h
    copter/pid.h
    joystick/joystickReader.h
    mixer/controlsMixer.h
    opencvUtils/opencvTransformations.h
    radio/frSkyMultimodule.h
    radio/multimoduleController.h
    radio/r9Module.h
    simulation/janibekovsBolt.h
    simulation/mainObject.h
    simulation/physMainObject.h
    simulation/physObject.h
    simulation/physSphere.h
    simulation/simObject.h
    simulation/simSphere.h
    simulation/simulation.h
    xml/generated/betaflightPIDParameters.h
    xml/generated/flightControllerParameters.h
    xml/generated/flightMode.h
    xml/generated/mixerChannelOperationParameters.h
    xml/generated/pIDParameters.h
    xml/generated/rateParameters.h
    xml/generated/sceneDrawBackendType.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    )

set(SOURCE_FILES
    clientSender.cpp
    comcontroller.cpp
    controlRecord.cpp
    copterInputs.cpp
    joystickInput.cpp
    )

set(AUTOPILOT_SOURCE_FILES
    autopilot/protoautopilot.cpp
    autopilot/vertexsquare.cpp  
    )

set(CALIBRATION_SOURCE_FILES
    calibration/calibration.cpp
    )

set(COPTER_SOURCE_FILES
    copter/droneObject.cpp
    copter/motor.cpp
    copter/quad.cpp
    copter/pid.cpp
    )

set(JOYSTICK_SOURCE_FILES
    joystick/joystickReader.cpp
    )

set(MIXER_SOURCE_FILE
    mixer/controlsMixer.cpp
    )

set(OPENCVUTILS_SOURCE_FILES
    opencvUtils/opencvTransformations.cpp
    )

set(RADIO_SOURCE_FILES
    radio/frSkyMultimodule.cpp
    radio/multimoduleController.cpp
    radio/r9Module.cpp
    )

set(SIMULATION_SOURCE_FILES
    simulation/dzhanibekovBolt.cpp
    simulation/mainObject.cpp
    simulation/physMainObject.cpp
    simulation/physObject.cpp
    simulation/physSphere.cpp
    simulation/simObject.cpp
    simulation/simSphere.cpp
    simulation/simulation.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    ${AUTOPILOT_SOURCE_FILES}
    ${CALIBRATION_SOURCE_FILES}
    ${COPTER_SOURCE_FILES}
    ${JOYSTICK_SOURCE_FILES}
    ${MIXER_SOURCE_FILE} 
    ${OPENCVUTILS_SOURCE_FILES} 
    ${RADIO_SOURCE_FILES} 
    ${SIMULATION_SOURCE_FILES} 
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
    corecvs
    corecvs_utils
    Qt5::SerialPort
    Qt5::Core
    Qt5::Gui
    JOYSTICKwrapper
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}/drone"
    )

