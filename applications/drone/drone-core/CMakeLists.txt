cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications/drone)
init_project(PROJECT_NAME drone-core)

find_package(Qt5 COMPONENTS REQUIRED SerialPort Core Gui Widgets)

set(PUBLIC_HEADER_FILES
    clientSender.h
    comcontroller.h
    controlRecord.h
    copterInputs.h
    autopilot/protoautopilot.h
    autopilot/vertexsquare.h
    calibration/calibration.h
    copter/droneObject.h
    copter/motor.h
    copter/quad.h
    copter/quadAngles.h
    copter/pid.h
    joystick/joystickReader.h
    mixer/controlsMixer.h
    opencvUtils/opencvTransformations.h
    radio/frSkyMultimodule.h
    radio/multimoduleController.h
    radio/r9Module.h
    simulation/mainObject.h
    simulation/physicsCompoundObject.h
    simulation/physicsObject.h
    simulation/physicsSphere.h
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
    world/simulationWorld.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    )

set(SOURCE_FILES
    clientSender.cpp
    comcontroller.cpp
    controlRecord.cpp
    copterInputs.cpp
    )

set(AUTOPILOT_SOURCE_FILES
    autopilot/protoautopilot.cpp
    autopilot/vertexsquare.cpp  
    )

set(CALIBRATION_SOURCE_FILE
    calibration/calibration.cpp
    )

set(COPTER_SOURCE_FILES
    copter/droneObject.cpp
    copter/motor.cpp
    copter/quad.cpp
    copter/quadAngles.cpp
    copter/pid.cpp
    )

set(JOYSTICK_SOURCE_FILE
    joystick/joystickReader.cpp
    )
    
set(MIXER_SOURCE_FILE
    mixer/controlsMixer.cpp
    )

set(OPENCVUTILS_SOURCE_FILE
    opencvUtils/opencvTransformations.cpp
    )

set(RADIO_SOURCE_FILES
    radio/frSkyMultimodule.cpp
    radio/multimoduleController.cpp
    radio/r9Module.cpp
    )

set(SIMULATION_SOURCE_FILES
    simulation/mainObject.cpp
    simulation/physicsCompoundObject.cpp
    simulation/physicsObject.cpp
    simulation/physicsSphere.cpp
    simulation/simObject.cpp
    simulation/simSphere.cpp
    simulation/simulation.cpp
    )

set(XML_SOURCE_FILES
    xml/generated/betaflightPIDParameters.cpp
    xml/generated/flightControllerParameters.cpp
    xml/generated/mixerChannelOperationParameters.cpp
    xml/generated/pIDParameters.cpp
    xml/generated/rateParameters.cpp
    )

set(WORLD_SOURCE_FILES
    world/simulationWorld.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    ${AUTOPILOT_SOURCE_FILES}
    ${CALIBRATION_SOURCE_FILE}
    ${COPTER_SOURCE_FILES}
    ${JOYSTICK_SOURCE_FILE}
    ${LEGACY_SOURCE_FILE}
    ${MIXER_SOURCE_FILE} 
    ${OPENCVUTILS_SOURCE_FILE} 
    ${RADIO_SOURCE_FILES} 
    ${SIMULATION_SOURCE_FILES}
    ${XML_SOURCE_FILES}
    ${WORLD_SOURCE_FILES}
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

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    corecvs
    corecvs_utils
    Qt5::SerialPort
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    JOYSTICKwrapper
    ${OpenCV_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )