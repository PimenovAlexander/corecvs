target_sources(corecvs
    PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/joystickInterface.h 
		${CMAKE_CURRENT_LIST_DIR}/playbackJoystickInterface.h
    PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/joystickInterface.cpp 
		${CMAKE_CURRENT_LIST_DIR}/playbackJoystickInterface.cpp
)

