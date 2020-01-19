CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

set (MODULE_NAME drone-ui)

project (drone-ui)


set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
#set(CMAKE_AUTOUIC "YES")
set(CMAKE_AUTORCC "YES")

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script)

add_library(drone-ui STATIC )


target_sources(drone-ui
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/calibration/calibrationWidget.h 
    ${CMAKE_CURRENT_LIST_DIR}/calibration/imageForCalibrationWidget.h 
    
    ${CMAKE_CURRENT_LIST_DIR}/joystick/JoystickOptionsWidget.h 
    ${CMAKE_CURRENT_LIST_DIR}/joystick/mixerChannelOperationWidget.h 
    
    ${CMAKE_CURRENT_LIST_DIR}/radio/radioControlWidget.h 
    ${CMAKE_CURRENT_LIST_DIR}/copterInputsWidget.h 
    
    ${CMAKE_CURRENT_LIST_DIR}/physicsMainWindow.h 
    ${CMAKE_CURRENT_LIST_DIR}/physicsAboutWidget.h 
    
    ${CMAKE_CURRENT_LIST_DIR}/frameProcessor.h

    ${CMAKE_CURRENT_LIST_DIR}/world/simulationWorld.h

)

target_sources(drone-ui
    PRIVATE

    ${CMAKE_CURRENT_LIST_DIR}/calibration/calibrationWidget.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/calibration/imageForCalibrationWidget.cpp 
    
    ${CMAKE_CURRENT_LIST_DIR}/joystick/JoystickOptionsWidget.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/joystick/mixerChannelOperationWidget.cpp 
    
    ${CMAKE_CURRENT_LIST_DIR}/radio/radioControlWidget.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/copterInputsWidget.cpp   
    
    ${CMAKE_CURRENT_LIST_DIR}/physicsMainWindow.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/physicsAboutWidget.cpp 

    ${CMAKE_CURRENT_LIST_DIR}/frameProcessor.cpp

    ${CMAKE_CURRENT_LIST_DIR}/world/simulationWorld.cpp
)

set (UI_FILES
    ${CMAKE_CURRENT_LIST_DIR}/physicsAboutWidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/radio/radioControlWidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/joystick/JoystickOptionsWidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/joystick/mixerChannelOperationWidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/physicsMainWindow.ui
    ${CMAKE_CURRENT_LIST_DIR}/calibration/imageForCalibrationWidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/calibration/calibrationWidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/copterInputsWidget.ui
    )

QT5_WRAP_UI( UI_HEADERS ${UI_FILES} )

#message(DRONE-UI: FILES:   ${UI_FILES})
#message(DRONE-UI: HEADERS: ${UI_HEADERS})



target_sources(drone-ui
    PRIVATE
    ${UI_HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}/drone.qrc
    )

target_include_directories(drone-ui PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/joystick
    ${CMAKE_CURRENT_LIST_DIR}/radio
    ${CMAKE_CURRENT_LIST_DIR}/mixer
    ${CMAKE_CURRENT_LIST_DIR}/autopilot
    ${CMAKE_CURRENT_LIST_DIR}/copter
    ${CMAKE_CURRENT_LIST_DIR}/calibration
    ${CMAKE_CURRENT_LIST_DIR}/opencvUtils
    ${CMAKE_CURRENT_LIST_DIR}/simulation

    ${CMAKE_CURRENT_LIST_DIR}/world
)


# Temporary fixes 
SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}_autogen/include")
target_include_directories(${MODULE_NAME} PUBLIC ${AUTOGEN_BUILD_DIR} )
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )

message("AUTOGEN_BUILD_DIR bin directory <${AUTOGEN_BUILD_DIR}>")

target_include_directories(drone-ui PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} ${drone-core_SOURCE_DIR} .)

target_link_libraries(drone-ui drone-core cvs_utils corecvs)

if (OpenCV_LIBS)
    target_link_libraries(drone-ui ${OpenCV_LIBS})
endif()

