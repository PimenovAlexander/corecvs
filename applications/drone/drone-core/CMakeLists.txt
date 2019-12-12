CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project (drone-core)

add_library(drone-core STATIC )


file(GLOB CURR_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/xml/generated/*.cpp)
file(GLOB CURR_HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/xml/generated/*.h)

set(SRC_FILES ${SRC_FILES} ${CURR_SRC_FILES})
set(HDR_FILES ${HDR_FILES} ${CURR_HDR_FILES})

set (INC_PATHS ${CMAKE_CURRENT_LIST_DIR}/xml/generated)

include(../../../wrappers/joystick/sourcelist.cmake)
include(../../../wrappers/jsonmodern/sourcelist.cmake)

target_sources(drone-core
    PUBLIC
        ${HDR_FILES}

        ${CMAKE_CURRENT_LIST_DIR}/calibration/calibration.h

        ${CMAKE_CURRENT_LIST_DIR}/copter/pid.h
        ${CMAKE_CURRENT_LIST_DIR}/copter/droneObject.h
        ${CMAKE_CURRENT_LIST_DIR}/copter/motor.h
        ${CMAKE_CURRENT_LIST_DIR}/copter/quad.h

        ${CMAKE_CURRENT_LIST_DIR}/simulation/dzhanibekovBolt.h
        ${CMAKE_CURRENT_LIST_DIR}/simulation/physMainObject.h
        ${CMAKE_CURRENT_LIST_DIR}/simulation/simulation.h
        ${CMAKE_CURRENT_LIST_DIR}/simulation/simSphere.h
        ${CMAKE_CURRENT_LIST_DIR}/simulation/simObject.h
        ${CMAKE_CURRENT_LIST_DIR}/simulation/mainObject.h
        ${CMAKE_CURRENT_LIST_DIR}/simulation/physObject.h
        ${CMAKE_CURRENT_LIST_DIR}/simulation/physSphere.h

        ${CMAKE_CURRENT_LIST_DIR}/mixer/controlsMixer.h

        ${CMAKE_CURRENT_LIST_DIR}/radio/frSkyMultimodule.h
        ${CMAKE_CURRENT_LIST_DIR}/radio/multimoduleController.h
        ${CMAKE_CURRENT_LIST_DIR}/radio/r9Module.h

        ${CMAKE_CURRENT_LIST_DIR}/autopilot/protoautopilot.h
        ${CMAKE_CURRENT_LIST_DIR}/autopilot/vertexsquare.h
        ${CMAKE_CURRENT_LIST_DIR}/opencvUtils/opencvTransformations.h

        ${CMAKE_CURRENT_LIST_DIR}/clientSender.h
        ${CMAKE_CURRENT_LIST_DIR}/joystickInput.h
        ${CMAKE_CURRENT_LIST_DIR}/controlRecord.h
        ${CMAKE_CURRENT_LIST_DIR}/comcontroller.h
        ${CMAKE_CURRENT_LIST_DIR}/copterInputs.h
        ${CMAKE_CURRENT_LIST_DIR}/joystick/joystickReader.h

    PRIVATE
        ${SRC_FILES}

        ${CMAKE_CURRENT_LIST_DIR}/calibration/calibration.cpp

        ${CMAKE_CURRENT_LIST_DIR}/copter/pid.cpp
        ${CMAKE_CURRENT_LIST_DIR}/copter/quad.cpp
        ${CMAKE_CURRENT_LIST_DIR}/copter/droneObject.cpp
        ${CMAKE_CURRENT_LIST_DIR}/copter/motor.cpp


        ${CMAKE_CURRENT_LIST_DIR}/simulation/physMainObject.cpp
        ${CMAKE_CURRENT_LIST_DIR}/simulation/simulation.cpp
        ${CMAKE_CURRENT_LIST_DIR}/simulation/simSphere.cpp
        ${CMAKE_CURRENT_LIST_DIR}/simulation/simObject.cpp
        ${CMAKE_CURRENT_LIST_DIR}/simulation/mainObject.cpp
        ${CMAKE_CURRENT_LIST_DIR}/simulation/physObject.cpp
        ${CMAKE_CURRENT_LIST_DIR}/simulation/physSphere.cpp
        ${CMAKE_CURRENT_LIST_DIR}/simulation/dzhanibekovBolt.cpp

        ${CMAKE_CURRENT_LIST_DIR}/mixer/controlsMixer.cpp

        ${CMAKE_CURRENT_LIST_DIR}/radio/frSkyMultimodule.cpp
        ${CMAKE_CURRENT_LIST_DIR}/radio/multimoduleController.cpp
        ${CMAKE_CURRENT_LIST_DIR}/radio/r9Module.cpp

        ${CMAKE_CURRENT_LIST_DIR}/autopilot/protoautopilot.cpp
        ${CMAKE_CURRENT_LIST_DIR}/autopilot/vertexsquare.cpp
        ${CMAKE_CURRENT_LIST_DIR}/opencvUtils/opencvTransformations.cpp

        ${CMAKE_CURRENT_LIST_DIR}/joystickInput.cpp
        ${CMAKE_CURRENT_LIST_DIR}/clientSender.cpp
        ${CMAKE_CURRENT_LIST_DIR}/controlRecord.cpp
        ${CMAKE_CURRENT_LIST_DIR}/copterInputs.cpp
        ${CMAKE_CURRENT_LIST_DIR}/comcontroller.cpp
        ${CMAKE_CURRENT_LIST_DIR}/joystick/joystickReader.cpp
)




target_include_directories(drone-core PUBLIC
        ${INC_PATHS}

        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/joystick
        ${CMAKE_CURRENT_LIST_DIR}/radio
        ${CMAKE_CURRENT_LIST_DIR}/mixer
        ${CMAKE_CURRENT_LIST_DIR}/autopilot
        ${CMAKE_CURRENT_LIST_DIR}/copter
        ${CMAKE_CURRENT_LIST_DIR}/calibration
        ${CMAKE_CURRENT_LIST_DIR}/opencvUtils
        ${CMAKE_CURRENT_LIST_DIR}/simulation
)

target_include_directories(drone-core PUBLIC ../../../wrappers/joystick)

find_package(Qt5 COMPONENTS REQUIRED SerialPort)
target_link_libraries(drone-core corecvs Qt5::SerialPort)


target_include_directories(drone-core PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)


target_link_libraries(drone-core cvs_utils corecvs)

if (OpenCV_LIBS)
    target_link_libraries(drone-core ${OpenCV_LIBS})
endif()

